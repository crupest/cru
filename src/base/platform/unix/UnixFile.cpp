#include "cru/base/platform/unix/UnixFile.h"
#include "cru/base/log/Logger.h"

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

namespace cru::platform::unix {

UnixFileDescriptor::UnixFileDescriptor()
    : descriptor_(-1), auto_close_(false) {}

UnixFileDescriptor::UnixFileDescriptor(int descriptor, bool auto_close,
                                       std::function<int(int)> close)
    : descriptor_(descriptor),
      auto_close_(auto_close),
      close_(std::move(close)) {}

UnixFileDescriptor::~UnixFileDescriptor() {
  constexpr auto kLogTag = "cru::platform::unix::UnixFileDescriptor";
  if (this->IsValid() && this->IsAutoClose()) {
    if (!this->DoClose()) {
      CruLogError(kLogTag, "Failed to close file descriptor {}, errno {}.",
                  descriptor_, errno);
    }
  }
}

UnixFileDescriptor::UnixFileDescriptor(UnixFileDescriptor&& other) noexcept
    : descriptor_(other.descriptor_),
      auto_close_(other.auto_close_),
      close_(std::move(other.close_)) {
  other.descriptor_ = -1;
  other.auto_close_ = false;
  other.close_ = nullptr;
}

UnixFileDescriptor& UnixFileDescriptor::operator=(
    UnixFileDescriptor&& other) noexcept {
  if (this != &other) {
    if (this->IsValid()) {
      this->Close();
    }
    this->descriptor_ = other.descriptor_;
    this->auto_close_ = other.auto_close_;
    this->close_ = other.close_;
    other.descriptor_ = -1;
    other.auto_close_ = false;
    other.close_ = nullptr;
  }
  return *this;
}

bool UnixFileDescriptor::IsValid() const { return this->descriptor_ >= 0; }

void UnixFileDescriptor::EnsureValid() const {
  if (!this->IsValid()) {
    throw Exception("Can't do operation on an invalid unix file descriptor.");
  }
}

int UnixFileDescriptor::GetValue() const {
  EnsureValid();
  return this->descriptor_;
}

void UnixFileDescriptor::Close() {
  EnsureValid();
  if (!this->DoClose()) {
    throw ErrnoException("Failed to call close on file descriptor.");
  }
  descriptor_ = -1;
  auto_close_ = false;
}

bool UnixFileDescriptor::DoClose() {
  if (this->close_) {
    return this->close_(this->descriptor_) == 0;
  } else {
    return ::close(this->descriptor_) == 0;
  }
}

ssize_t UnixFileDescriptor::Read(void* buffer, size_t size) {
  EnsureValid();
  auto result = ::read(GetValue(), buffer, size);
  if (result == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return -1;
    } else {
      throw ErrnoException("Failed to read on file descriptor.");
    }
  }
  return result;
}

ssize_t UnixFileDescriptor::Write(const void* buffer, size_t size) {
  EnsureValid();
  auto result = ::write(GetValue(), buffer, size);
  if (result == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return -1;
    } else {
      throw ErrnoException("Failed to write on file descriptor.");
    }
  }
  return result;
}

void UnixFileDescriptor::SetFileDescriptorFlags(int flags) {
  EnsureValid();
  if (::fcntl(GetValue(), F_SETFL, flags) == -1) {
    throw ErrnoException("Failed to set flags on file descriptor.");
  }
}

UniDirectionalUnixPipeResult OpenUniDirectionalPipe(UnixPipeFlag flags) {
  int fds[2];
  if (::pipe(fds) != 0) {
    throw ErrnoException("Failed to create unix pipe.");
  }

  UnixFileDescriptor read(fds[0]), write(fds[1]);

  if (flags & UnixPipeFlags::NonBlock) {
    read.SetFileDescriptorFlags(O_NONBLOCK);
    write.SetFileDescriptorFlags(O_NONBLOCK);
  }

  return {std::move(read), std::move(write)};
}
}  // namespace cru::platform::unix
