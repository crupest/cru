#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_UNIX

#include "../../Base.h"
#include "../../SubProcess.h"
#include "../../io/AutoReadStream.h"

#include "UnixFileStream.h"
#include "UnixPipe.h"

#include <spawn.h>

namespace cru::platform::unix {
class PosixSpawnSubProcess : public PlatformSubProcessBase {
  CRU_DEFINE_CLASS_LOG_TAG(u"PosixSpawnSubProcess")

 public:
  explicit PosixSpawnSubProcess(const SubProcessStartInfo& start_info);
  ~PosixSpawnSubProcess();

  io::Stream* GetStdinStream() override;
  io::Stream* GetStdoutStream() override;
  io::Stream* GetStderrStream() override;

 protected:
  void PlatformCreateProcess() override;
  SubProcessExitResult PlatformWaitForProcess() override;
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
