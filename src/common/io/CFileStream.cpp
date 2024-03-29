#include "cru/common/io/CFileStream.h"
#include "cru/common/Exception.h"
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
  StreamOperationNotSupportedException::CheckRead(readable_);
  auto count = std::fread(buffer + offset, 1, size, file_);
  return count;
}

bool CFileStream::CanWrite() {
  CheckClosed();
  return writable_;
}

Index CFileStream::Write(const std::byte* buffer, Index offset, Index size) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckWrite(writable_);
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
  StreamAlreadyClosedException::Check(file_ == nullptr);
}
}  // namespace cru::io
