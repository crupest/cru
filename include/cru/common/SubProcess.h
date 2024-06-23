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
  String program;
  std::vector<String> arguments;
  std::unordered_map<String, String> environments;
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

template <typename T>
concept PlatformSubProcessImpl =
    requires(T process, const SubProcessStartInfo& start_info) {
      /**
       * @brief Default constructible.
       */
      new T();

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
      process.PlatformCreateProcess(start_info);

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
      {
        process.PlatformWaitForProcess()
      } -> std::same_as<SubProcessExitResult>;

      /**
       * @brief Kill the process immediately.
       *
       * This method will be called only once on this thread given
       * `PlatformCreateProcess` returns successfully. There will be a window
       * that the window already exits but the status has not been updated and
       * this is called. So handle this gracefully and write to internal data
       * carefully.
       */
      process.PlatformKillProcess();

      { process.GetStdinStream() } -> std::convertible_to<io::Stream*>;
      { process.GetStdoutStream() } -> std::convertible_to<io::Stream*>;
      { process.GetStderrStream() } -> std::convertible_to<io::Stream*>;
    };

struct IPlatformSubProcess : virtual Interface {
  /**
   * @brief Create and start a real process. If the process can't be created or
   * start, `SubProcessFailedToStartException` will be thrown. If this function
   * is already called once, `SubProcessException` will be thrown. Ensure only
   * call this method once.
   */
  virtual void Start() = 0;

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
  virtual void Wait(std::optional<std::chrono::milliseconds> wait_time) = 0;

  /**
   * @brief kill the process if it is running. If the process already exits,
   * nothing will happen. If the process has not started or failed to start,
   * `SubProcessException` will be throw. Ensure `Start` is called and does not
   * throw before calling this.
   */
  virtual void Kill() = 0;

  /**
   * @brief Get the status of the process.
   * 1. If the process has tried to start, aka `Start` is called, then this
   * method will return one of `Running`, `FailedToStart`, `Exited`.
   * 2. If it returns `Prepare`, `Start` is not called.
   * 3. It does NOT guarantee that this return `Running` and the process is
   * actually running. Because there might be a window that the process exits
   * already but status is not updated.
   */
  virtual SubProcessStatus GetStatus() = 0;

  /**
   * @brief Get the exit result. If the process is not started, failed to start
   * or running, `SubProcessException` will be thrown.
   */
  virtual SubProcessExitResult GetExitResult() = 0;

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
template <PlatformSubProcessImpl Impl>
class PlatformSubProcess : public Object, public virtual IPlatformSubProcess {
  CRU_DEFINE_CLASS_LOG_TAG(u"PlatformSubProcessBase")

 private:
  struct State {
    explicit State(SubProcessStartInfo start_info)
        : start_info(std::move(start_info)) {}

    std::mutex mutex;
    std::unique_lock<std::mutex> lock{mutex, std::defer_lock};
    std::condition_variable condition_variable;
    SubProcessStartInfo start_info;
    SubProcessExitResult exit_result;
    SubProcessStatus status = SubProcessStatus::Prepare;
    bool killed = false;
    Impl impl;
  };

 public:
  explicit PlatformSubProcess(SubProcessStartInfo start_info)
      : state_(new State(std::move(start_info))) {}

  ~PlatformSubProcess() override {}

  /**
   * @brief Create and start a real process. If the process can't be created or
   * start, `SubProcessFailedToStartException` will be thrown. If this function
   * is already called once, `SubProcessException` will be thrown. Ensure only
   * call this method once.
   */
  void Start() override {
    std::lock_guard lock_guard(this->state_->lock);

    if (this->state_->status != SubProcessStatus::Prepare) {
      throw SubProcessException(u"The process has already tried to start.");
    }

    try {
      this->state_->impl.PlatformCreateProcess(this->state_->start_info);
      this->state_->status = SubProcessStatus::Running;

      auto thread = std::thread([state = state_] {
        std::lock_guard lock_guard(state->lock);
        state->exit_result = state->impl.PlatformWaitForProcess();
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
  void Wait(std::optional<std::chrono::milliseconds> wait_time) override {
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

  /**
   * @brief kill the process if it is running. If the process already exits,
   * nothing will happen. If the process has not started or failed to start,
   * `SubProcessException` will be throw. Ensure `Start` is called and does not
   * throw before calling this.
   */
  void Kill() override {
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

    this->state_->impl.PlatformKillProcess();
    this->state_->killed = true;
  }

  /**
   * @brief Get the status of the process.
   * 1. If the process has tried to start, aka `Start` is called, then this
   * method will return one of `Running`, `FailedToStart`, `Exited`.
   * 2. If it returns `Prepare`, `Start` is not called.
   * 3. It does NOT guarantee that this return `Running` and the process is
   * actually running. Because there might be a window that the process exits
   * already but status is not updated.
   */
  SubProcessStatus GetStatus() override {
    std::lock_guard lock_guard(this->state_->lock);
    return this->state_->status;
  }

  /**
   * @brief Get the exit result. If the process is not started, failed to start
   * or running, `SubProcessException` will be thrown.
   */
  SubProcessExitResult GetExitResult() override {
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

  io::Stream* GetStdinStream() override {
    return this->state_->impl.GetStdinStream();
  }
  io::Stream* GetStdoutStream() override {
    return this->state_->impl.GetStdoutStream();
  }
  io::Stream* GetStderrStream() override {
    return this->state_->impl.GetStderrStream();
  }

 private:
  std::shared_ptr<State> state_;
};

class CRU_BASE_API SubProcess : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"SubProcess")

 public:
  static SubProcess Create(
      String program, std::vector<String> arguments = {},
      std::unordered_map<String, String> environments = {});

 public:
  SubProcess(SubProcessStartInfo start_info);

  CRU_DELETE_COPY(SubProcess)

  SubProcess(SubProcess&& other) = default;
  SubProcess& operator=(SubProcess&& other) = default;

  ~SubProcess();

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
  std::unique_ptr<IPlatformSubProcess> platform_process_;
};
}  // namespace cru
