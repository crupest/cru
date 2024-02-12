#include "cru/common/platform/unix/UnixPipe.h"
#include "cru/common/Exception.h"
#include "cru/common/log/Logger.h"

#include <fcntl.h>
#include <sys/fcntl.h>
#include <unistd.h>

namespace cru::platform::unix {
UnixPipe::UnixPipe(Usage usage, UnixPipeFlag flags)
    : usage_(usage), flags_(flags) {
  int fds[2];
  if (pipe(fds) != 0) {
    throw ErrnoException(u"Failed to create unix pipe.");
  }

  if (flags & UnixPipeFlags::NonBlock) {
    fcntl(fds[0], F_SETFL, O_NONBLOCK);
    fcntl(fds[1], F_SETFL, O_NONBLOCK);
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
