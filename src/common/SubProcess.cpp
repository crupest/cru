#include "cru/common/SubProcess.h"

#include <thread>

#ifdef CRU_PLATFORM_UNIX
#include "cru/common/platform/unix/PosixSpawnSubProcess.h"
#endif

namespace cru {

#ifdef CRU_PLATFORM_UNIX
using ThisPlatformSubProcessImpl = platform::unix::PosixSpawnSubProcessImpl;
#endif

PlatformSubProcess::PlatformSubProcess(
    SubProcessStartInfo start_info,
    std::shared_ptr<IPlatformSubProcessImpl> impl)
    : state_(new State(std::move(start_info), std::move(impl))) {}

PlatformSubProcess::~PlatformSubProcess() {}

void PlatformSubProcess::Start() {
  std::lock_guard lock_guard(this->state_->lock);

  if (this->state_->status != SubProcessStatus::Prepare) {
    throw SubProcessException(u"The process has already tried to start.");
  }

  try {
    this->state_->impl->PlatformCreateProcess(this->state_->start_info);
    this->state_->status = SubProcessStatus::Running;

    auto thread = std::thread([state = state_] {
      std::lock_guard lock_guard(state->lock);
      state->exit_result = state->impl->PlatformWaitForProcess();
      state->status = SubProcessStatus::Exited;
      state->condition_variable.notify_all();
    });

    thread.detach();
  } catch (const std::exception& e) {
    this->state_->status = SubProcessStatus::FailedToStart;
    throw SubProcessFailedToStartException(u"Sub-process failed to start. " +
                                           String::FromUtf8(e.what()));
  }
}

void PlatformSubProcess::Wait(
    std::optional<std::chrono::milliseconds> wait_time) {
  std::lock_guard lock_guard(this->state_->lock);

  if (this->state_->status == SubProcessStatus::Prepare) {
    throw SubProcessException(
        u"The process does not start. Can't wait for it.");
  }

  if (this->state_->status == SubProcessStatus::FailedToStart) {
    throw SubProcessException(
        u"The process failed to start. Can't wait for it.");
  }

  if (this->state_->status == SubProcessStatus::Exited) {
    return;
  }

  auto predicate = [this] {
    return this->state_->status == SubProcessStatus::Exited;
  };

  if (wait_time) {
    this->state_->condition_variable.wait_for(this->state_->lock, *wait_time,
                                              predicate);
  } else {
    this->state_->condition_variable.wait(this->state_->lock, predicate);
  }
}

void PlatformSubProcess::Kill() {
  std::lock_guard lock_guard(this->state_->lock);

  if (this->state_->status == SubProcessStatus::Prepare) {
    throw SubProcessException(u"The process does not start. Can't kill it.");
  }

  if (this->state_->status == SubProcessStatus::FailedToStart) {
    throw SubProcessException(u"The process failed to start. Can't kill it.");
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
  std::lock_guard lock_guard(this->state_->lock);
  return this->state_->status;
}

SubProcessExitResult PlatformSubProcess::GetExitResult() {
  std::lock_guard lock_guard(this->state_->lock);

  if (this->state_->status == SubProcessStatus::Prepare) {
    throw SubProcessException(
        u"The process does not start. Can't get exit result.");
  }

  if (this->state_->status == SubProcessStatus::FailedToStart) {
    throw SubProcessException(
        u"The process failed to start. Can't get exit result.");
  }

  if (this->state_->status == SubProcessStatus::Running) {
    throw SubProcessException(
        u"The process is running. Can't get exit result.");
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

SubProcess SubProcess::Create(String program, std::vector<String> arguments,
                              std::unordered_map<String, String> environments) {
  SubProcessStartInfo start_info;
  start_info.program = std::move(program);
  start_info.arguments = std::move(arguments);
  start_info.environments = std::move(environments);
  return SubProcess(std::move(start_info));
}

SubProcess::SubProcess(SubProcessStartInfo start_info) {
  platform_process_.reset(new PlatformSubProcess(
      std::move(start_info), std::make_shared<ThisPlatformSubProcessImpl>()));
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
    throw SubProcessException(u"SubProcess instance is invalid.");
  }
}

}  // namespace cru
