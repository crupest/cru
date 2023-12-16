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
  FailToStart,
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

/**
 * @brief Base class of a platform process. It is one-time, which means it
 * starts and exits and can't start again.
 */
class PlatformSubProcessBase : public Object {
 public:
  explicit PlatformSubProcessBase(
      const PlatformSubProcessStartInfo& start_info);

  ~PlatformSubProcessBase() override;

  /**
   * @brief Create and start a real process. If the program can't be created or
   * start, an exception should be thrown.
   */
  void Start();
  /**
   * @brief Wait for the process to exit for at most `wait_time`.
   */
  void Wait(std::optional<std::chrono::milliseconds> wait_time);
  PlatformSubProcessStatus GetStatus() const;
  int GetExitCode() const;
  io::Stream* GetStdinStream();
  io::Stream* GetStdoutStream();
  io::Stream* GetStderrStream();

  void Join();
  void Detach();

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
   * @brief Wait for the created process forever. After process exits, fill the
   * result info of internal data.
   *
   * Implementation should wait for the real process forever.
   *
   * This method will be called on another thread. It will only called once
   * after a success call of `Start`. It is safe to write internal data in this
   * method because process lock will be hold and we won't write to internal.
   */
  virtual void PlatformWaitForProcess() = 0;

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
  enum class DisposeKind {
    None,
    Join,
    Detach,
  };

  DisposeKind dispose_;

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
