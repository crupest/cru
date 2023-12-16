#include "cru/common/SubProcess.h"
#include "cru/common/Exception.h"

#include <mutex>

namespace cru {
SubProcessException::SubProcessException(String message)
    : Exception(std::move(message)) {}

SubProcessException::~SubProcessException() {}

SubProcessFailedToStartException::SubProcessFailedToStartException(
    String message)
    : Exception(std::move(message)) {}

SubProcessFailedToStartException::~SubProcessFailedToStartException() {}

PlatformSubProcessBase::PlatformSubProcessBase(
    const PlatformSubProcessStartInfo& start_info)
    : start_info_(start_info), process_lock_(process_mutex_, std::defer_lock) {}

PlatformSubProcessBase::~PlatformSubProcessBase() {}

void PlatformSubProcessBase::Start() {
  std::lock_guard lock_guard(process_lock_);

  if (status_ != PlatformSubProcessStatus::Prepare) {
    throw SubProcessException(u"The process has already tried to start.");
  }

  try {
    PlatformCreateProcess();

    status_ = PlatformSubProcessStatus::Running;

    process_thread_ = std::thread([this] {
      auto exit_result = PlatformWaitForProcess();
      {
        std::lock_guard lock_guard(process_lock_);
        exit_result_ = std::move(exit_result);
        status_ = PlatformSubProcessStatus::Exited;
      }
      this->process_condition_variable_.notify_all();
    });

    process_thread_.detach();
  } catch (const std::exception& e) {
    status_ = PlatformSubProcessStatus::FailedToStart;
    throw SubProcessFailedToStartException(u"Sub-process failed to start. " +
                                           String::FromUtf8(e.what()));
  }
}

void PlatformSubProcessBase::Wait(
    std::optional<std::chrono::milliseconds> wait_time) {
  std::lock_guard lock_guard(process_lock_);

  if (status_ == PlatformSubProcessStatus::Prepare) {
    throw SubProcessException(
        u"The process does not start. Can't wait for it.");
  }

  if (status_ == PlatformSubProcessStatus::FailedToStart) {
    throw SubProcessException(
        u"The process failed to start. Can't wait for it.");
  }

  if (status_ == PlatformSubProcessStatus::Exited) {
    return;
  }

  auto predicate = [this] {
    return status_ == PlatformSubProcessStatus::Exited;
  };

  if (wait_time) {
    process_condition_variable_.wait_for(process_lock_, *wait_time, predicate);
  } else {
    process_condition_variable_.wait(process_lock_, predicate);
  }
}

void PlatformSubProcessBase::Kill() {
  auto status = GetStatus();

  if (status == PlatformSubProcessStatus::Prepare) {
    throw SubProcessException(u"The process does not start. Can't kill it.");
  }

  if (status == PlatformSubProcessStatus::FailedToStart) {
    throw SubProcessException(u"The process failed to start. Can't kill it.");
  }

  if (status == PlatformSubProcessStatus::Exited) {
    return;
  }

  PlatformKillProcess();
}

PlatformSubProcessStatus PlatformSubProcessBase::GetStatus() {
  std::lock_guard data_lock_guard(process_lock_);
  return status_;
}

PlatformSubProcessExitResult PlatformSubProcessBase::GetExitResult() {
  std::lock_guard lock_guard(process_lock_);

  if (status_ == PlatformSubProcessStatus::Prepare) {
    throw SubProcessException(
        u"The process does not start. Can't get exit result.");
  }

  if (status_ == PlatformSubProcessStatus::FailedToStart) {
    throw SubProcessException(
        u"The process failed to start. Can't get exit result.");
  }

  if (status_ == PlatformSubProcessStatus::Running) {
    throw SubProcessException(
        u"The process is running. Can't get exit result.");
  }

  return exit_result_;
}
}  // namespace cru
