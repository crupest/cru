#pragma once
#include "Base.h"
#include "Exception.h"
#include "String.h"
#include "io/Stream.h"

#include <chrono>
#include <condition_variable>
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

class CRU_BASE_API SubProcessFailedToStartException : public Exception {
 public:
  SubProcessFailedToStartException(String message = {});
  ~SubProcessFailedToStartException() override;
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
 * If an object of this class is destructed before the process exits, the
 * process will be killed.
 */
class PlatformSubProcessBase : public Object {
 public:
  explicit PlatformSubProcessBase(
      const PlatformSubProcessStartInfo& start_info);

  ~PlatformSubProcessBase() override;

  /**
   * @brief Create and start a real process. If the process can't be created or
   * start, `SubProcessFailedToStartException` will be thrown. If this function
   * is already called once, `SubProcessException` will be thrown. Ensure only
   * call this method once.
   */
  void Start();

  /**
   * @brief Wait for the process to exit optionally for at most `wait_time`. If
   * the process already exits, it will return immediately. If the process has
   * not started or failed to start, `SubProcessException` will be thrown.
   * Ensure `Start` is called and does not throw before calling this.
   *
   * @remarks You may wish this returns bool to indicate whether it is timeout
   * or the process exits. But no, even if it is timeout, the process may also
   * have exited due to task schedule.
   */
  void Wait(std::optional<std::chrono::milliseconds> wait_time);

  /**
   * @brief kill the process if it is running. If the process already exits,
   * nothing will happen. If the process has not started or failed to start,
   * `SubProcessException` will be throw. Ensure `Start` is called and does not
   * throw before calling this.
   */
  void Kill();

  /**
   * @brief Get the status of the process.
   * 1. If the process has tried to start, aka `Start` is called, then this
   * method will return one of `Running`, `FailedToStart`, `Exited`.
   * 2. If it returns `Prepare`, `Start` is not called.
   * 3. It does NOT guarantee that this return `Running` and the process is
   * actually running. Because there might be a window that the process exits
   * already but status is not updated.
   */
  PlatformSubProcessStatus GetStatus();

  /**
   * @brief Get the exit result. If the process is not started, failed to start
   * or running, `SubProcessException` will be thrown.
   */
  PlatformSubProcessExitResult GetExitResult();

  io::Stream* GetStdinStream();
  io::Stream* GetStdoutStream();
  io::Stream* GetStderrStream();

 protected:
  /**
   * @brief Create and start a real process. If the program can't be created or
   * start, an exception should be thrown.
   *
   * Implementation should fill internal data of the new process and start it.
   *
   * This method will be called only once in first call of `Start` on this
   * thread with lock hold.
   */
  virtual void PlatformCreateProcess() = 0;

  /**
   * @brief Wait for the created process forever.
   *
   * Implementation should wait for the real process forever, after that, fill
   * internal data and returns exit result.
   *
   * This method will be called only once on another thread after
   * `PlatformCreateProcess` returns successfully
   */
  virtual PlatformSubProcessExitResult PlatformWaitForProcess() = 0;

  /**
   * @brief Kill the process immediately.
   *
   * This method will be called only once on this thread given
   * `PlatformCreateProcess` returns successfullyThere will be a window that the
   * window already exits but the status has not been updated and this is
   * called. So handle this gracefully and write to internal data carefully.
   */
  virtual void PlatformKillProcess() = 0;

 protected:
  PlatformSubProcessStartInfo start_info_;
  PlatformSubProcessExitResult exit_result_;

 private:
  PlatformSubProcessStatus status_;

  std::thread process_thread_;
  std::mutex process_mutex_;
  std::unique_lock<std::mutex> process_lock_;
  std::condition_variable process_condition_variable_;
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
