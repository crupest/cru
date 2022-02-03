#include "cru/common/platform/unix/UnixFileStream.hpp"
#include "cru/common/Format.hpp"
#include "cru/common/io/OpenFileFlag.hpp"
#include "cru/common/platform/unix/ErrnoException.hpp"

#include <fcntl.h>
#include <sys/_types/_s_ifmt.h>
#include <unistd.h>

namespace cru::platform::unix {
using namespace cru::io;

namespace {
int MapOpenFileFlag(OpenFileFlag flags) {
  int result = 0;
  if (flags & OpenFileFlags::Read) {
    if (flags & OpenFileFlags::Write) {
      result |= O_RDWR;
    } else {
      result |= O_RDONLY;
    }
  } else {
    if (flags & OpenFileFlags::Write) {
      result |= O_WRONLY;
    } else {
      throw Exception(u"Invalid open file flag.");
    }
  }

  if (flags & OpenFileFlags::Append) {
    result |= O_APPEND;
  }

  if (flags & OpenFileFlags::Create) {
    result |= O_CREAT;
  }

  if (flags & OpenFileFlags::ThrowOnExist) {
    result |= O_EXCL;
  }

  return result;
}

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

UnixFileStream::~UnixFileStream() { Close(); }

UnixFileStream::UnixFileStream(String path, OpenFileFlag flags)
    : path_(std::move(path)), flags_(flags) {
  auto p = path_.ToUtf8();
  file_descriptor_ =
      ::open(p.c_str(), MapOpenFileFlag(flags_), S_IRUSR | S_IWUSR);
  if (file_descriptor_ == -1) {
    throw ErrnoException(
        Format(u"Failed to open file {} with flags {}.", path_, flags_.value));
  }
}

bool UnixFileStream::CanSeek() {
  CheckClosed();
  return true;
}

Index UnixFileStream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();
  off_t result = ::lseek(file_descriptor_, offset, MapSeekOrigin(origin));
  if (result == -1) {
    throw ErrnoException(u"Failed to seek file.");
  }
  return result;
}

bool UnixFileStream::CanRead() {
  CheckClosed();
  return flags_ & OpenFileFlags::Read;
}

Index UnixFileStream::Read(std::byte *buffer, Index offset, Index size) {
  CheckClosed();
  auto result = ::read(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to read file.");
  }
  return result;
}

bool UnixFileStream::CanWrite() {
  CheckClosed();
  return flags_ & OpenFileFlags::Write;
}

Index UnixFileStream::Write(const std::byte *buffer, Index offset, Index size) {
  CheckClosed();
  auto result = ::write(file_descriptor_, buffer + offset, size);
  if (result == -1) {
    throw ErrnoException(u"Failed to write file.");
  }
  return result;
}

void UnixFileStream::Close() {
  if (closed_) return;
  if (::close(file_descriptor_) == -1) {
    throw ErrnoException(u"Failed to close file.");
  }
  closed_ = true;
}

void UnixFileStream::CheckClosed() {
  if (closed_) {
    throw Exception(u"File is closed.");
  }
}
}  // namespace cru::platform::unix
