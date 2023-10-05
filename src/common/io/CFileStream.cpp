#include "cru/common/io/CFileStream.h"
#include "cru/common/Exception.h"
#include "cru/common/io/OpenFileFlag.h"
#include "cru/common/io/Stream.h"

#include <cstdio>

namespace cru::io {
static bool ModeCanRead(const char* mode) {
  for (const char* p = mode; *p != '\0'; p++) {
    if (*p == 'r' || *p == '+') {
      return true;
    }
  }
  return false;
}

static bool ModeCanWrite(const char* mode) {
  for (const char* p = mode; *p != '\0'; p++) {
    if (*p == 'w' || *p == 'a' || *p == '+') {
      return true;
    }
  }
  return false;
}

CFileStream::CFileStream(const char* path, const char* mode)
    : file_(std::fopen(path, mode)),
      readable_(ModeCanRead(mode)),
      writable_(ModeCanWrite(mode)),
      auto_close_(true) {
  if (file_ == nullptr) {
    throw ErrnoException(u"Cannot open file.");
  }
}

CFileStream::CFileStream(std::FILE* file, bool readable, bool writable,
                         bool auto_close)
    : file_(file),
      readable_(readable),
      writable_(writable),
      auto_close_(auto_close) {
  if (file_ == nullptr) {
    throw Exception(u"File is NULL.");
  }
}

namespace {
/**
 *  Generally the fopen will return a NULL ptr if the file does not exist. Then
 * we must throw FileNotExistException. However, there are cases we have to
 * check existence explicitly before. The case is OpenFileFlags::Write is
 * specified but OpenFileFlags::Create is not, in which fopen usually create a
 * new file automatically but we want a FileNotExistException to be thrown.
 */
std::string ConvertOpenFileFlagToCFileFlag(OpenFileFlag flags,
                                           bool* explicit_check_exist) {
  *explicit_check_exist = false;

  std::string result;

  bool need_read = flags & OpenFileFlags::Read;
  bool need_write = flags & OpenFileFlags::Write;

  if (!need_write) {
    // No need to write? The simplest
    // Note even OpenFileFlags::Create is set, we still have to check exist.
    return "r";
  }

  // Now we need writing.

  bool create = OpenFileFlags::Create;

  if (!create) {
    *explicit_check_exist = true;
  }

  bool append = flags & OpenFileFlags::Append;

  if (!need_read) {
    return "w";
  }
}
}  // namespace

CFileStream::CFileStream(String path, OpenFileFlag flags) {}

CFileStream::~CFileStream() {
  if (auto_close_ && file_ != nullptr) {
    std::fclose(file_);
  }
}

bool CFileStream::CanSeek() {
  CheckClosed();
  return true;
}

static int ConvertOriginFlag(Stream::SeekOrigin origin) {
  switch (origin) {
    case Stream::SeekOrigin::Begin:
      return SEEK_SET;
    case Stream::SeekOrigin::Current:
      return SEEK_CUR;
    case Stream::SeekOrigin::End:
      return SEEK_END;
    default:
      throw Exception(u"Unknown seek origin.");
  }
}

Index CFileStream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();
  if (std::fseek(file_, offset, ConvertOriginFlag(origin))) {
    throw ErrnoException(u"Seek failed.");
  }
  return Tell();
}

Index CFileStream::Tell() {
  CheckClosed();
  long position = std::ftell(file_);
  if (position == -1) {
    throw ErrnoException(u"Tell failed.");
  }
  return position;
}

void CFileStream::Rewind() {
  CheckClosed();
  std::rewind(file_);
}

bool CFileStream::CanRead() {
  CheckClosed();
  return readable_;
}

Index CFileStream::Read(std::byte* buffer, Index offset, Index size) {
  CheckClosed();
  auto count = std::fread(buffer + offset, 1, size, file_);
  return count;
}

bool CFileStream::CanWrite() {
  CheckClosed();
  return writable_;
}

Index CFileStream::Write(const std::byte* buffer, Index offset, Index size) {
  CheckClosed();
  auto count = std::fwrite(buffer + offset, 1, size, file_);
  return count;
}

void CFileStream::Flush() {
  CheckClosed();
  std::fflush(file_);
}

void CFileStream::Close() {
  if (file_ != nullptr) {
    std::fclose(file_);
    file_ = nullptr;
  }
}

void CFileStream::CheckClosed() {
  if (file_ == nullptr) {
    throw StreamAlreadyClosedException(u"File is closed.");
  }
}
}  // namespace cru::io
