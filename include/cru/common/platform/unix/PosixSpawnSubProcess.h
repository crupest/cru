#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_UNIX

#include "../../SubProcess.h"

#include "UnixFileStream.h"
#include "UnixPipe.h"

#include <spawn.h>

namespace cru::platform::unix {
class PosixSpawnSubProcess : public PlatformSubProcessBase {
 public:
  explicit PosixSpawnSubProcess(const PlatformSubProcessStartInfo& start_info);
  ~PosixSpawnSubProcess();

 private:
  pid_t pid_;
  int exit_code_;

  UnixPipe stdin_pipe_;
  UnixPipe stdout_pipe_;
  UnixPipe stderr_pipe_;

  std::unique_ptr<UnixFileStream> stdin_stream_;
  std::unique_ptr<UnixFileStream> stdout_stream_;
  std::unique_ptr<UnixFileStream> stderr_stream_;
};
}  // namespace cru::platform::unix

#endif
