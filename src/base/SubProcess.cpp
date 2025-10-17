#include "cru/base/SubProcess.h"

#include <thread>

#ifdef CRU_PLATFORM_UNIX
#include "cru/base/platform/unix/PosixSpawnSubProcess.h"
#endif

namespace cru {
PlatformSubProcess::PlatformSubProcess(
    SubProcessStartInfo start_info,
    std::shared_ptr<IPlatformSubProcessImpl> impl)
    : state_(new State(std::move(start_info), std::move(impl))),
      lock_(state_->mutex, std::defer_lock) {}

PlatformSubProcess::~PlatformSubProcess() {}

void PlatformSubProcess::Start() {
  std::lock_guard lock_guard(this->lock_);

  if (this->state_->status != SubProcessStatus::Prepare) {
    throw SubProcessException("The process has already tried to start.");
  }

  try {
    this->state_->impl->PlatformCreateProcess(this->state_->start_info);
    this->state_->status = SubProcessStatus::Running;

    auto thread = std::thread([state = state_] {
      std::unique_lock lock(state->mutex);
      state->exit_result = state->impl->PlatformWaitForProcess();
      state->status = SubProcessStatus::Exited;
      state->condition_variable.notify_all();
    });

    thread.detach();
  } catch (const std::exception& e) {
    this->state_->status = SubProcessStatus::FailedToStart;
    throw SubProcessFailedToStartException(
        std::string("Sub-process failed to start. ") + e.what());
  }
}

void PlatformSubProcess::Wait(
    std::optional<std::chrono::milliseconds> wait_time) {
  std::lock_guard lock_guard(this->lock_);

  if (this->state_->status == SubProcessStatus::Prepare) {
    throw SubProcessException("The process does not start. Can't wait for it.");
  }

  if (this->state_->status == SubProcessStatus::FailedToStart) {
    throw SubProcessException(
        "The process failed to start. Can't wait for it.");
  }

  if (this->state_->status == SubProcessStatus::Exited) {
    return;
  }

  auto predicate = [this] {
    return this->state_->status == SubProcessStatus::Exited;
  };

  if (wait_time) {
    this->state_->condition_variable.wait_for(this->lock_, *wait_time,
                                              predicate);
  } else {
    this->state_->condition_variable.wait(this->lock_, predicate);
  }
}

void PlatformSubProcess::Kill() {
  std::lock_guard lock_guard(this->lock_);

  if (this->state_->status == SubProcessStatus::Prepare) {
    throw SubProcessException("The process does not start. Can't kill it.");
  }

  if (this->state_->status == SubProcessStatus::FailedToStart) {
    throw SubProcessException("The process failed to start. Can't kill it.");
  }

  if (this->state_->status == SubProcessStatus::Exited) {
    return;
  }

  if (this->state_->killed) {
    return;
  }

  this->state_->impl->PlatformKillProcess();
  this->state_->killed = true;
}

SubProcessStatus PlatformSubProcess::GetStatus() {
  std::lock_guard lock_guard(this->lock_);
  return this->state_->status;
}

SubProcessExitResult PlatformSubProcess::GetExitResult() {
  std::lock_guard lock_guard(this->lock_);

  if (this->state_->status == SubProcessStatus::Prepare) {
    throw SubProcessException(
        "The process does not start. Can't get exit result.");
  }

  if (this->state_->status == SubProcessStatus::FailedToStart) {
    throw SubProcessException(
        "The process failed to start. Can't get exit result.");
  }

  if (this->state_->status == SubProcessStatus::Running) {
    throw SubProcessException("The process is running. Can't get exit result.");
  }

  return this->state_->exit_result;
}

io::Stream* PlatformSubProcess::GetStdinStream() {
  return this->state_->impl->GetStdinStream();
}

io::Stream* PlatformSubProcess::GetStdoutStream() {
  return this->state_->impl->GetStdoutStream();
}

io::Stream* PlatformSubProcess::GetStderrStream() {
  return this->state_->impl->GetStderrStream();
}

SubProcess SubProcess::Create(
    std::string program, std::vector<std::string> arguments,
    std::unordered_map<std::string, std::string> environments) {
  SubProcessStartInfo start_info;
  start_info.program = std::move(program);
  start_info.arguments = std::move(arguments);
  start_info.environments = std::move(environments);
  return SubProcess(std::move(start_info));
}

SubProcessExitResult SubProcess::Call(
    std::string program, std::vector<std::string> arguments,
    std::unordered_map<std::string, std::string> environments) {
  auto process =
      Create(std::move(program), std::move(arguments), std::move(environments));
  process.Wait();
  return process.GetExitResult();
}

SubProcess::SubProcess(SubProcessStartInfo start_info) {
#ifdef CRU_PLATFORM_UNIX
  platform_process_.reset(new PlatformSubProcess(
      std::move(start_info),
      std::make_shared<platform::unix::PosixSpawnSubProcessImpl>()));
#else
  NotImplemented();
#endif
  platform_process_->Start();
}

SubProcess::~SubProcess() {}

void SubProcess::Wait(std::optional<std::chrono::milliseconds> wait_time) {
  CheckValid();
  platform_process_->Wait(wait_time);
}

void SubProcess::Kill() {
  CheckValid();
  platform_process_->Kill();
}

SubProcessStatus SubProcess::GetStatus() {
  CheckValid();
  return platform_process_->GetStatus();
}

SubProcessExitResult SubProcess::GetExitResult() {
  CheckValid();
  return platform_process_->GetExitResult();
}

io::Stream* SubProcess::GetStdinStream() {
  CheckValid();
  return platform_process_->GetStdinStream();
}

io::Stream* SubProcess::GetStdoutStream() {
  CheckValid();
  return platform_process_->GetStdoutStream();
}

io::Stream* SubProcess::GetStderrStream() {
  CheckValid();
  return platform_process_->GetStderrStream();
}

void SubProcess::Detach() { auto p = platform_process_.release(); }

void SubProcess::CheckValid() const {
  if (!IsValid()) {
    throw SubProcessException("SubProcess instance is invalid.");
  }
}

}  // namespace cru
