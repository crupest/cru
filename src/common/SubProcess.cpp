#include "cru/common/SubProcess.h"
#include "cru/common/Exception.h"

#include <mutex>

namespace cru {
SubProcessException::SubProcessException(String message)
    : Exception(std::move(message)) {}

SubProcessException::~SubProcessException() {}

PlatformSubProcessBase::PlatformSubProcessBase(
    const PlatformSubProcessStartInfo& start_info)
    : auto_delete_(false),
      start_info_(start_info),
      data_lock_(data_mutex_, std::defer_lock),
      process_lock_(process_mutex_, std::defer_lock) {}

PlatformSubProcessBase::~PlatformSubProcessBase() {}

void PlatformSubProcessBase::Start() {
  auto data_lock_guard = CreateDataLockGuard();

  if (status_ != PlatformSubProcessStatus::Prepare) {
    throw SubProcessException(u"Sub-process has already run.");
  }

  status_ = PlatformSubProcessStatus::Running;

  try {
    PlatformCreateProcess();

    process_thread_ = std::thread([this] {
      auto process_lock_guard = CreateProcessLockGuard();
      PlatformWaitForProcess();
      auto data_lock_guard = CreateDataLockGuard();
      status_ = PlatformSubProcessStatus::Exited;
    });

    process_thread_.detach();
  } catch (const std::exception& e) {
    status_ = PlatformSubProcessStatus::FailedToStart;
  }
}

void PlatformSubProcessBase::Wait(
    std::optional<std::chrono::milliseconds> wait_time) {
  auto status = GetStatus();

  if (status == PlatformSubProcessStatus::Prepare) {
    throw SubProcessException(
        u"The process does not start. Can't wait for it.");
  }

  if (status == PlatformSubProcessStatus::FailedToStart) {
    throw SubProcessException(
        u"The process failed to start. Can't wait for it.");
  }

  if (status == PlatformSubProcessStatus::Exited) {
    return;
  }

  if (wait_time) {
    auto locked = process_lock_.try_lock_for(*wait_time);
    if (locked) {
      process_lock_.unlock();
    }
  } else {
    process_lock_.lock();
    process_lock_.unlock();
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
  auto data_lock_guard = CreateDataLockGuard();
  return status_;
}
}  // namespace cru
