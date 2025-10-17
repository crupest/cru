#pragma once
#include "Base.h"
#include "Exception.h"
#include "io/Stream.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

namespace cru {
enum class SubProcessStatus {
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
  using Exception::Exception;
};

class CRU_BASE_API SubProcessFailedToStartException
    : public SubProcessException {
 public:
  using SubProcessException::SubProcessException;
};

class CRU_BASE_API SubProcessInternalException : public SubProcessException {
 public:
  using SubProcessException::SubProcessException;
};

struct SubProcessStartInfo {
  std::string program;
  std::vector<std::string> arguments;
  std::unordered_map<std::string, std::string> environments;
};

enum class SubProcessExitType {
  Unknown,
  Normal,
  Signal,
};

struct SubProcessExitResult {
  SubProcessExitType exit_type;
  int exit_code;
  int exit_signal;
  bool has_core_dump;

  bool IsSuccess() const {
    return exit_type == SubProcessExitType::Normal && exit_code == 0;
  }

  static SubProcessExitResult Unknown() {
    return {SubProcessExitType::Unknown, 0, 0, false};
  }

  static SubProcessExitResult Normal(int exit_code) {
    return {SubProcessExitType::Normal, exit_code, 0, false};
  }

  static SubProcessExitResult Signal(int exit_signal, bool has_core_dump) {
    return {SubProcessExitType::Normal, 0, exit_signal, has_core_dump};
  }
};

struct IPlatformSubProcessImpl : virtual Interface {
  /**
   * @brief Create and start a real process.
   *
   * If the program can't be created or start, an exception should be
   * thrown.
   *
   * Implementation should fill internal data of the new process and start
   * it.
   *
   * This method will be called only once in first call of `Start` on this
   * thread with lock holdDefaultConstructible.
   */
  virtual void PlatformCreateProcess(const SubProcessStartInfo& start_info) = 0;

  /**
   * @brief Wait for the created process forever and return the exit result
   * when process stops.
   *
   * Implementation should wait for the real process forever, after that,
   * fill internal data and returns exit result.
   *
   * This method will be called only once on another thread after
   * `PlatformCreateProcess` returns successfully
   */
  virtual SubProcessExitResult PlatformWaitForProcess() = 0;

  /**
   * @brief Kill the process immediately.
   *
   * This method will be called only once on this thread given
   * `PlatformCreateProcess` returns successfully. There will be a window
   * that the window already exits but the status has not been updated and
   * this is called. So handle this gracefully and write to internal data
   * carefully.
   */
  virtual void PlatformKillProcess() = 0;

  virtual io::Stream* GetStdinStream() = 0;
  virtual io::Stream* GetStdoutStream() = 0;
  virtual io::Stream* GetStderrStream() = 0;
};

/**
 * @brief A wrapper platform process. It is one-time, which means it
 * starts and exits and can't start again.
 *
 * TODO: Current implementation has a problem. If the process does not exit for
 * a long time, the resource related to it will not be released. It may cause a
 * leak.
 */
class PlatformSubProcess : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("PlatformSubProcess")

 private:
  struct State {
    explicit State(SubProcessStartInfo start_info,
                   std::shared_ptr<IPlatformSubProcessImpl> impl)
        : start_info(std::move(start_info)), impl(std::move(impl)) {}

    std::mutex mutex;
    std::condition_variable condition_variable;
    SubProcessStartInfo start_info;
    SubProcessExitResult exit_result;
    SubProcessStatus status = SubProcessStatus::Prepare;
    bool killed = false;
    std::shared_ptr<IPlatformSubProcessImpl> impl;
  };

 public:
  PlatformSubProcess(SubProcessStartInfo start_info,
                     std::shared_ptr<IPlatformSubProcessImpl> impl);

  ~PlatformSubProcess() override;

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
  SubProcessStatus GetStatus();

  /**
   * @brief Get the exit result. If the process is not started, failed to start
   * or running, `SubProcessException` will be thrown.
   */
  SubProcessExitResult GetExitResult();

  io::Stream* GetStdinStream();
  io::Stream* GetStdoutStream();
  io::Stream* GetStderrStream();

 private:
  std::shared_ptr<State> state_;
  std::unique_lock<std::mutex> lock_;
};

class CRU_BASE_API SubProcess : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("SubProcess")

 public:
  static SubProcess Create(
      std::string program, std::vector<std::string> arguments = {},
      std::unordered_map<std::string, std::string> environments = {});

  static SubProcessExitResult Call(
      std::string program, std::vector<std::string> arguments = {},
      std::unordered_map<std::string, std::string> environments = {});

 public:
  SubProcess(SubProcessStartInfo start_info);

  CRU_DELETE_COPY(SubProcess)

  SubProcess(SubProcess&& other) = default;
  SubProcess& operator=(SubProcess&& other) = default;

  ~SubProcess() override;

 public:
  void Wait(std::optional<std::chrono::milliseconds> wait_time = std::nullopt);
  void Kill();

  SubProcessStatus GetStatus();
  SubProcessExitResult GetExitResult();

  io::Stream* GetStdinStream();
  io::Stream* GetStdoutStream();
  io::Stream* GetStderrStream();

  void Detach();

  bool IsValid() const { return platform_process_ != nullptr; }
  explicit operator bool() const { return IsValid(); }

 private:
  void CheckValid() const;

 private:
  std::unique_ptr<PlatformSubProcess> platform_process_;
};
}  // namespace cru
