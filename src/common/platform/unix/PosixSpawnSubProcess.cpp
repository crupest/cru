#include "cru/common/platform/unix/PosixSpawnSubProcess.h"
#include "cru/common/SubProcess.h"

#include <spawn.h>

namespace cru::platform::unix {
PosixSpawnSubProcess::PosixSpawnSubProcess(
    const PlatformSubProcessStartInfo& start_info)
    : PlatformSubProcessBase(start_info), pid_(0), exit_code_(0) {}

PosixSpawnSubProcess::~PosixSpawnSubProcess() {}

}  // namespace cru::platform::unix
