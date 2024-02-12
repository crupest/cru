#include "cru/common/platform/unix/PosixSpawnSubProcess.h"
#include "cru/common/SubProcess.h"

#include <spawn.h>
#include <memory>

namespace cru::platform::unix {
PosixSpawnSubProcess::PosixSpawnSubProcess(
    const PlatformSubProcessStartInfo& start_info)
    : PlatformSubProcessBase(start_info),
      pid_(0),
      exit_code_(0),
      stdin_pipe_(UnixPipe::Usage::Send),
      stdout_pipe_(UnixPipe::Usage::Receive),
      stderr_pipe_(UnixPipe::Usage::Receive) {
  stdin_stream_ = std::make_unique<UnixFileStream>(
      stdin_pipe_.GetSelfFileDescriptor(), false, false, true, false);
  stdout_stream_ = std::make_unique<UnixFileStream>(
      stdout_pipe_.GetSelfFileDescriptor(), false, true, false, false);
  stderr_stream_ = std::make_unique<UnixFileStream>(
      stderr_pipe_.GetSelfFileDescriptor(), false, true, false, false);
}

PosixSpawnSubProcess::~PosixSpawnSubProcess() {}

void PosixSpawnSubProcess::PlatformCreateProcess() {
  
}

PlatformSubProcessExitResult PosixSpawnSubProcess::PlatformWaitForProcess() {}

void PosixSpawnSubProcess::PlatformKillProcess() {}
}  // namespace cru::platform::unix
