#pragma once

#if !defined(__unix)
#error "This file can only be included on unix."
#endif

#include "../../Base.h"
#include "../../SubProcess.h"
#include "../../io/AutoReadStream.h"

#include "UnixFileStream.h"
#include "UnixFile.h"

#include <spawn.h>

namespace cru::platform::unix {
class PosixSpawnSubProcessImpl : public Object,
                                 public virtual IPlatformSubProcessImpl {
  CRU_DEFINE_CLASS_LOG_TAG(u"PosixSpawnSubProcess")

 public:
  explicit PosixSpawnSubProcessImpl();
  ~PosixSpawnSubProcessImpl();

  void PlatformCreateProcess(const SubProcessStartInfo& start_info) override;
  SubProcessExitResult PlatformWaitForProcess() override;
  void PlatformKillProcess() override;

  io::Stream* GetStdinStream() override;
  io::Stream* GetStdoutStream() override;
  io::Stream* GetStderrStream() override;

 private:
  pid_t pid_;
  int exit_code_;

  std::unique_ptr<UnixFileStream> stdin_stream_;
  std::unique_ptr<UnixFileStream> stdout_stream_;
  std::unique_ptr<UnixFileStream> stderr_stream_;

  std::unique_ptr<io::AutoReadStream> stdout_buffer_stream_;
  std::unique_ptr<io::AutoReadStream> stderr_buffer_stream_;
};
}  // namespace cru::platform::unix
