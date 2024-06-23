#pragma once

#include "../../PreConfig.h"

#ifndef CRU_PLATFORM_UNIX
#error "This file can only be included on unix."
#endif

#include "../../Base.h"
#include "../../SubProcess.h"
#include "../../io/AutoReadStream.h"

#include "UnixFileStream.h"
#include "UnixPipe.h"

#include <spawn.h>

namespace cru::platform::unix {
class PosixSpawnSubProcessImpl {
  CRU_DEFINE_CLASS_LOG_TAG(u"PosixSpawnSubProcess")

 public:
  explicit PosixSpawnSubProcessImpl();
  ~PosixSpawnSubProcessImpl();

  io::Stream* GetStdinStream();
  io::Stream* GetStdoutStream();
  io::Stream* GetStderrStream();

  void PlatformCreateProcess(const SubProcessStartInfo& start_info);
  SubProcessExitResult PlatformWaitForProcess();
  void PlatformKillProcess();

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

using PosixSpawnSubProcess = PlatformSubProcess<PosixSpawnSubProcessImpl>;
}  // namespace cru::platform::unix
