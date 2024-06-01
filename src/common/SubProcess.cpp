#include "cru/common/SubProcess.h"
#include "cru/common/Exception.h"

#include <mutex>

namespace cru {
PlatformSubProcessBase::PlatformSubProcessBase(
    const SubProcessStartInfo& start_info)
    : start_info_(start_info), process_lock_(process_mutex_, std::defer_lock) {}

PlatformSubProcessBase::~PlatformSubProcessBase() {}

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
}  // namespace cru
