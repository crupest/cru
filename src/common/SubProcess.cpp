#include "cru/common/SubProcess.h"
#include "cru/common/Exception.h"

#include <mutex>

namespace cru {
SubProcessException::SubProcessException(String message)
    : Exception(std::move(message)) {}

SubProcessException::~SubProcessException() {}

PlatformSubProcessBase::PlatformSubProcessBase(
    const PlatformSubProcessStartInfo& start_info)
    : dispose_(DisposeKind::None),
      start_info_(start_info),
      data_lock_(data_mutex_, std::defer_lock),
      process_lock_(process_mutex_, std::defer_lock) {}

PlatformSubProcessBase::~PlatformSubProcessBase() {
  auto data_lock_guard = CreateDataLockGuard();
  if (status_ )
    switch (dispose_) { DisposeKind::Join: }
}

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
  } catch (const std::exception& e) {
    status_ = PlatformSubProcessStatus::FailToStart;
  }
}

void PlatformSubProcessBase::Wait(
    std::optional<std::chrono::milliseconds> wait_time) {
  if (wait_time) {
    process_lock_.try_lock_for(*wait_time);
  } else {
    process_lock_.lock();
  }
}

}  // namespace cru
