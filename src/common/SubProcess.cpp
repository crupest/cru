#include "cru/common/SubProcess.h"
#include <exception>
#include "cru/common/Exception.h"
#include "cru/common/log/Logger.h"

#ifdef CRU_PLATFORM_UNIX
#include "cru/common/platform/unix/PosixSpawnSubProcess.h"
#endif

#include <mutex>

namespace cru {
PlatformSubProcessBase::PlatformSubProcessBase(SubProcessStartInfo start_info)
    : start_info_(std::move(start_info)),
      delete_self_(false),
      process_lock_(process_mutex_, std::defer_lock) {}

PlatformSubProcessBase::~PlatformSubProcessBase() {
  std::lock_guard lock_guard(process_lock_);
  if (status_ == SubProcessStatus::Running) {
    CRU_LOG_ERROR(
        u"PlatformSubProcessBase is destroyed but process is still running.");
    std::terminate();
  }
}

void PlatformSubProcessBase::Start() {
  std::lock_guard lock_guard(process_lock_);

  if (status_ != SubProcessStatus::Prepare) {
    throw SubProcessException(u"The process has already tried to start.");
  }

  try {
    PlatformCreateProcess();

    status_ = SubProcessStatus::Running;

    process_thread_ = std::thread([this] {
      auto exit_result = PlatformWaitForProcess();
      {
        std::lock_guard lock_guard(process_lock_);
        exit_result_ = std::move(exit_result);
        status_ = SubProcessStatus::Exited;
      }
      this->process_condition_variable_.notify_all();
      if (this->delete_self_) {
        delete this;
      }
    });

    process_thread_.detach();
  } catch (const std::exception& e) {
    status_ = SubProcessStatus::FailedToStart;
    throw SubProcessFailedToStartException(u"Sub-process failed to start. " +
                                           String::FromUtf8(e.what()));
  }
}

void PlatformSubProcessBase::Wait(
    std::optional<std::chrono::milliseconds> wait_time) {
  std::lock_guard lock_guard(process_lock_);

  if (status_ == SubProcessStatus::Prepare) {
    throw SubProcessException(
        u"The process does not start. Can't wait for it.");
  }

  if (status_ == SubProcessStatus::FailedToStart) {
    throw SubProcessException(
        u"The process failed to start. Can't wait for it.");
  }

  if (status_ == SubProcessStatus::Exited) {
    return;
  }

  auto predicate = [this] { return status_ == SubProcessStatus::Exited; };

  if (wait_time) {
    process_condition_variable_.wait_for(process_lock_, *wait_time, predicate);
  } else {
    process_condition_variable_.wait(process_lock_, predicate);
  }
}

void PlatformSubProcessBase::Kill() {
  std::lock_guard data_lock_guard(process_lock_);

  if (status_ == SubProcessStatus::Prepare) {
    throw SubProcessException(u"The process does not start. Can't kill it.");
  }

  if (status_ == SubProcessStatus::FailedToStart) {
    throw SubProcessException(u"The process failed to start. Can't kill it.");
  }

  if (status_ == SubProcessStatus::Exited) {
    return;
  }

  PlatformKillProcess();
}

SubProcessStatus PlatformSubProcessBase::GetStatus() {
  std::lock_guard data_lock_guard(process_lock_);
  return status_;
}

SubProcessExitResult PlatformSubProcessBase::GetExitResult() {
  std::lock_guard lock_guard(process_lock_);

  if (status_ == SubProcessStatus::Prepare) {
    throw SubProcessException(
        u"The process does not start. Can't get exit result.");
  }

  if (status_ == SubProcessStatus::FailedToStart) {
    throw SubProcessException(
        u"The process failed to start. Can't get exit result.");
  }

  if (status_ == SubProcessStatus::Running) {
    throw SubProcessException(
        u"The process is running. Can't get exit result.");
  }

  return exit_result_;
}

void PlatformSubProcessBase::SetDeleteSelfOnExit(bool enable) {
  std::lock_guard lock_guard(process_lock_);
  delete_self_ = enable;
}

#ifdef CRU_PLATFORM_UNIX
using PlatformSubProcess = platform::unix::PosixSpawnSubProcess;
#endif

SubProcess SubProcess::Create(String program, std::vector<String> arguments,
                              std::unordered_map<String, String> environments) {
  SubProcessStartInfo start_info;
  start_info.program = std::move(program);
  start_info.arguments = std::move(arguments);
  start_info.environments = std::move(environments);
  return SubProcess(std::move(start_info));
}

SubProcess::SubProcess(SubProcessStartInfo start_info) {
  platform_process_.reset(new PlatformSubProcess(std::move(start_info)));
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

void SubProcess::Detach() {
  auto p = platform_process_.release();
  p->SetDeleteSelfOnExit(true);
}

void SubProcess::CheckValid() const {
  if (!IsValid()) {
    throw SubProcessException(u"SubProcess instance is invalid.");
  }
}

}  // namespace cru
