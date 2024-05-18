#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_UNIX

#include "../../SubProcess.h"
#include "../../io/AutoReadStream.h"

#include "UnixFileStream.h"
#include "UnixPipe.h"

#include <spawn.h>

namespace cru::platform::unix {
class PosixSpawnSubProcess : public PlatformSubProcessBase {
 public:
  explicit PosixSpawnSubProcess(const PlatformSubProcessStartInfo& start_info);
  ~PosixSpawnSubProcess();

  io::Stream* GetStdinStream() override;
  io::Stream* GetStdoutStream() override;
  io::Stream* GetStderrStream() override;

 protected:
  void PlatformCreateProcess() override;
  PlatformSubProcessExitResult PlatformWaitForProcess() override;
  void PlatformKillProcess() override;

 private:
  pid_t pid_;
  int exit_code_;

  UnixPipe stdin_pipe_;
  UnixPipe stdout_pipe_;
  UnixPipe stderr_pipe_;

  std::unique_ptr<UnixFileStream> stdin_stream_;
  std::unique_ptr<UnixFileStream> stdout_stream_;
  std::unique_ptr<UnixFileStream> stderr_stream_;

  std::unique_ptr<io::AutoReadStream> stdout_buffer_stream_;
  std::unique_ptr<io::AutoReadStream> stderr_buffer_stream_;
};
}  // namespace cru::platform::unix

#endif
