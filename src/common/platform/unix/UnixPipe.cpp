#include "cru/common/platform/unix/UnixPipe.h"
#include "cru/common/Exception.h"
#include "cru/common/log/Logger.h"

#include <unistd.h>

namespace cru::platform::unix {
UnixPipe::UnixPipe(Usage usage) : usage_(usage) {
  int fds[2];
  if (pipe(fds) != 0) {
    throw ErrnoException(u"Failed to create unix pipe.");
  }
  read_fd_ = fds[0];
  write_fd_ = fds[1];
}

int UnixPipe::GetSelfFileDescriptor() {
  if (usage_ == Usage::Send) {
    return write_fd_;
  } else {
    return read_fd_;
  }
}

int UnixPipe::GetOtherFileDescriptor() {
  if (usage_ == Usage::Send) {
    return read_fd_;
  } else {
    return write_fd_;
  }
}

UnixPipe::~UnixPipe() {
  if (close(GetSelfFileDescriptor()) != 0) {
    CRU_LOG_ERROR(u"Failed to close unix pipe file descriptor.");
  }
}
}  // namespace cru::platform::unix
