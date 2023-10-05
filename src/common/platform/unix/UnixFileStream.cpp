#include "cru/common/platform/unix/UnixFileStream.h"
#include "cru/common/Format.h"
#include "cru/common/io/Stream.h"
#include "cru/common/platform/unix/ErrnoException.h"

#include <fcntl.h>
#include <sys/fcntl.h>
#include <unistd.h>

namespace cru::platform::unix {
using namespace cru::io;

namespace {
bool OflagCanSeek([[maybe_unused]] int oflag) {
  // Treat every file seekable.
  return true;
}

bool OflagCanRead(int oflag) { return oflag & O_RDONLY || oflag & O_RDWR; }

bool OflagCanWrite(int oflag) { return oflag & O_WRONLY || oflag & O_RDWR; }

int MapSeekOrigin(Stream::SeekOrigin origin) {
  switch (origin) {
    case Stream::SeekOrigin::Begin:
      return SEEK_SET;
    case Stream::SeekOrigin::Current:
      return SEEK_CUR;
    case Stream::SeekOrigin::End:
      return SEEK_END;
    default:
      throw Exception(u"Invalid seek origin.");
  }
}
}  // namespace

UnixFileStream::UnixFileStream(const char *path, int oflag) {
  file_descriptor_ = ::open(path, oflag);
  if (file_descriptor_ == -1) {
    throw ErrnoException(Format(u"Failed to open file {} with oflag {}.",
                                String::FromUtf8(path), oflag));
  }

  can_seek_ = OflagCanSeek(oflag);
  can_read_ = OflagCanRead(oflag);
  can_write_ = OflagCanWrite(oflag);
  auto_close_ = true;
}

UnixFileStream::UnixFileStream(int fd, bool can_seek, bool can_read,
                               bool can_write, bool auto_close) {
  file_descriptor_ = fd;
  can_seek_ = can_seek;
  can_read_ = can_read;
  can_write_ = can_write;
  auto_close_ = auto_close;
}

UnixFileStream::~UnixFileStream() { Close(); }

bool UnixFileStream::CanSeek() {
  CheckClosed();
  return can_seek_;
}

Index UnixFileStream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckSeek(can_seek_);
  off_t result = ::lseek(file_descriptor_, offset, MapSeekOrigin(origin));
  if (result == -1) {
    throw ErrnoException(u"Failed to seek file.");
  }
  return result;
}

bool UnixFileStream::CanRead() {
  CheckClosed();
  return can_read_;
}

Index UnixFileStream::Read(std::byte *buffer, Index offset, Index size) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckRead(can_read_);
  auto result = ::read(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to read file.");
  }
  return result;
}

bool UnixFileStream::CanWrite() {
  CheckClosed();
  return can_write_;
}

Index UnixFileStream::Write(const std::byte *buffer, Index offset, Index size) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckWrite(can_write_);
  auto result = ::write(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to write file.");
  }
  return result;
}

void UnixFileStream::Close() {
  if (file_descriptor_ < 0) return;
  if (::close(file_descriptor_) == -1) {
    throw ErrnoException(u"Failed to close file.");
  }
  file_descriptor_ = -1;
}

void UnixFileStream::CheckClosed() {
  StreamAlreadyClosedException::Check(file_descriptor_ < 0);
}
}  // namespace cru::platform::unix
