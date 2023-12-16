#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_UNIX

#include "../../SubProcess.h"

#include <spawn.h>

namespace cru::platform::unix {
class PosixSpawnSubProcess : public PlatformSubProcessBase {
 public:
  explicit PosixSpawnSubProcess(const PlatformSubProcessStartInfo& start_info);
  ~PosixSpawnSubProcess();

 private:
  pid_t pid_;
  int exit_code_;
};
}  // namespace cru::platform::unix

#endif
