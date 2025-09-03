#include "cru/base/platform/unix/Timer.h"

namespace cru::platform::unix {
int UnixTimerFile::GetReadFd() const { return this->read_fd_; }
}  // namespace cru::platform::unix
