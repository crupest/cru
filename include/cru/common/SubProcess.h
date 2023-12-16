#pragma once
#include "Base.h"
#include "String.h"

#include <chrono>
#include <optional>
#include <vector>

namespace cru {
enum class PlatformSubProcessStatus {
  /**
   * @brief The process has not been created and started.
   */
  Prepare,
  /**
   * @brief The process is running now.
   */
  Running,
  /**
   * @brief The process has been exited.
   */
  Exited,
};

/**
 * Interface of a platform process. It is one-time, which means it starts and
 * exits and can't start again.
 */
struct IPlatformSubProcess : virtual Interface {
  /**
   * Create and start a real process.
   */
  void Start(String program, std::vector<String> arguments,
             std::vector<String> environments);
  void Wait(std::optional<std::chrono::milliseconds> wait_time);
  void Reap();
  PlatformSubProcessStatus GetStatus() const;
  int GetExitCode() const;
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
