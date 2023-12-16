#pragma once
#include "Base.h"
#include "Exception.h"
#include "String.h"
#include "io/Stream.h"

#include <chrono>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace cru {
enum class PlatformSubProcessStatus {
  /**
   * @brief The process has not been created and started.
   */
  Prepare,
  /**
   * @brief The process is failed to start.
   */
  FailedToStart,
  /**
   * @brief The process is running now.
   */
  Running,
  /**
   * @brief The process has been exited.
   */
  Exited,
};

class CRU_BASE_API SubProcessException : public Exception {
 public:
  SubProcessException(String message = {});
  ~SubProcessException() override;
};

struct PlatformSubProcessStartInfo {
  String program;
  std::vector<String> arguments;
  std::vector<String> environments;
};

struct PlatformSubProcessExitResult {
  int exit_code;
};

/**
 * @brief Base class of a platform process. It is one-time, which means it
 * starts and exits and can't start again.
 * @remarks
 * If an object of this class is destructed before the process ends, the process
 * will be killed.
 */
class PlatformSubProcessBase : public Object {
 public:
  explicit PlatformSubProcessBase(
      const PlatformSubProcessStartInfo& start_info);

  ~PlatformSubProcessBase() override;

  /**
   * @brief Create and start a real process. If the program can't be created or
   * start, an exception will be thrown.
   *
   * @remarks This method will hold the data lock during running. It ensures
   * after return, the process already tries to start and status is definitely
   * not `Prepare` but `FailedToStart` or `Running` or `Exit`. So it is safe to
   * use `GetStatus` to check whether the process tries to start, aka, this
   * method has been called or not.
   */
  void Start();

  /**
   * @brief Wait for the process to exit for at most `wait_time`. If the process
   * already exits, it will return immediately. If the process has not started
   * or failed to start, it will throw. Ensure `Start` is called and does not
   * throw before call this.
   *
   * @remarks You may wish this returns bool to indicate whether it is timeout
   * or the process exits. But no, even if it is timeout, the process may also
   * exits due to task schedule.
   */
  void Wait(std::optional<std::chrono::milliseconds> wait_time);

  /**
   * @brief kill the process if it is running. If the process already exits,
   * nothing will happen. If the process has not started or failed to start, it
   * will throw. Ensure `Start` is called and does not throw before call this.
   */
  void Kill();

  /**
   * @brief Get the status of the process.
   * 1. If the process has tried to start, aka `Start` is called, then this
   * method must return `Running`, `FailedToStart`.
   * 2. If it returns `Prepare`, `Start` is not called.
   * 3. It does NOT guarantee that this return `Running` and the process is
   * actually running. Because there might be a window that the process exits
   * already but status is not updated.
   */
  PlatformSubProcessStatus GetStatus();

  PlatformSubProcessExitResult GetExitResult();
  io::Stream* GetStdinStream();
  io::Stream* GetStdoutStream();
  io::Stream* GetStderrStream();

  bool IsAutoDeleteAfterProcessExit() const;

  /**
   * @brief If auto delete, this instance will delete it self after the process
   * exits. The delete will be called at another thread. You must ensure this
   * object is created by new.
   */
  void SetAutoDeleteAfterProcessExit(bool auto_delete);

 protected:
  /**
   * @brief Create and start a real process. If the program can't be created or
   * start, an exception should be thrown.
   *
   * Implementation should fill internal data of the new process and start it.
   *
   * This method will be called on this thread with data lock hold. It will only
   * called once in first call of `Start`.
   */
  virtual void PlatformCreateProcess() = 0;

  /**
   * @brief Wait for the created process forever.
   *
   * Implementation should wait for the real process forever, after that, fill
   * internal data and returns exit result.
   *
   * This method will be called on another thread. It will only be called once
   * after a success call of `Start`. It is safe to write internal data in this
   * method because process lock will be hold and we won't write to internal.
   */
  virtual PlatformSubProcessExitResult PlatformWaitForProcess() = 0;

  /**
   * @brief Kill the process immediately.
   *
   * This method will be called on this thread. It will only be called once
   * after a success call of `Start`. There will be a window that the window
   * already exits but the status has not been updated and this is called. So
   * handle this gracefully and write to internal data carefully.
   */
  virtual void PlatformKillProcess() = 0;

 private:
  auto CreateDataLockGuard() {
    return std::lock_guard<std::unique_lock<std::mutex>>(data_lock_);
  }

  auto CreateProcessLockGuard() {
    return std::lock_guard<std::unique_lock<std::timed_mutex>>(process_lock_);
  }

 protected:
  PlatformSubProcessStartInfo start_info_;

 private:
  bool auto_delete_;

  PlatformSubProcessStatus status_;

  std::mutex data_mutex_;
  /**
   * Lock for protecting data of this class.
   */
  std::unique_lock<std::mutex> data_lock_;
  std::thread process_thread_;

  std::timed_mutex process_mutex_;
  /**
   * Lock for protecting internal data of sub-class, and used for detect whether
   * process is running.
   */
  std::unique_lock<std::timed_mutex> process_lock_;
};

class CRU_BASE_API SubProcess : public Object {
 public:
  SubProcess();

  CRU_DELETE_COPY(SubProcess)

  SubProcess(SubProcess&& other);
  SubProcess& operator=(SubProcess&& other);

  ~SubProcess();

 private:
};
}  // namespace cru
