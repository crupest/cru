#include "cru/base/platform/unix/UnixFileStream.h"
#include "cru/base/Exception.h"
#include "cru/base/io/Stream.h"

#include <fcntl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <format>

namespace cru::platform::unix {
using namespace cru::io;

namespace {
bool OflagCanSeek([[maybe_unused]] int oflag) {
  // Treat every file seekable.
  return true;
}

bool OflagCanRead(int oflag) {
  // There is a problem: O_RDONLY is 0. So we must test it specially.
  // If it is not write-only. Then it can read.
  return !(oflag & O_WRONLY);
}

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

UnixFileStream::UnixFileStream(const char *path, int oflag, mode_t mode) {
  file_descriptor_ = UnixFileDescriptor(::open(path, oflag, mode));
  if (file_descriptor_ == -1) {
    throw ErrnoException(std::format(
        "Failed to open file {} with oflag {}, mode {}.", path, oflag, mode));
  }

  SetSupportedOperations(
      {OflagCanSeek(oflag), OflagCanRead(oflag), OflagCanWrite(oflag)});
}

UnixFileStream::UnixFileStream(UnixFileDescriptor fd, bool can_seek,
                               bool can_read, bool can_write)
    : Stream(can_seek, can_read, can_write), file_descriptor_(std::move(fd)) {}

UnixFileStream::~UnixFileStream() { DoClose(); }

Index UnixFileStream::DoSeek(Index offset, SeekOrigin origin) {
  off_t result = ::lseek(file_descriptor_, offset, MapSeekOrigin(origin));
  if (result == -1) {
    throw ErrnoException(u"Failed to seek file.");
  }
  return result;
}

Index UnixFileStream::DoRead(std::byte *buffer, Index offset, Index size) {
  auto result = ::read(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to read file.");
  }
  return result;
}

Index UnixFileStream::DoWrite(const std::byte *buffer, Index offset,
                              Index size) {
  auto result = ::write(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to write file.");
  }
  return result;
}

void UnixFileStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE
  if (file_descriptor_) {
    file_descriptor_ = {};
  }
}
}  // namespace cru::platform::unix
