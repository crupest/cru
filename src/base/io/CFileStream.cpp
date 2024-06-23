#include "cru/base/io/CFileStream.h"
#include "cru/base/Exception.h"
#include "cru/base/io/Stream.h"

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
    : Stream(true, ModeCanRead(mode), ModeCanWrite(mode)),
      file_(std::fopen(path, mode)),
      auto_close_(true) {
  if (file_ == nullptr) {
    throw ErrnoException(u"Cannot open file.");
  }
}

CFileStream::CFileStream(std::FILE* file, bool readable, bool writable,
                         bool auto_close)
    : Stream(true, readable, writable), file_(file), auto_close_(auto_close) {
  if (file_ == nullptr) {
    throw Exception(u"File is NULL.");
  }
}

CFileStream::~CFileStream() {
  if (auto_close_ && file_ != nullptr) {
    std::fclose(file_);
  }
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

Index CFileStream::DoSeek(Index offset, SeekOrigin origin) {
  if (std::fseek(file_, offset, ConvertOriginFlag(origin))) {
    throw ErrnoException(u"Seek failed.");
  }
  return DoTell();
}

Index CFileStream::DoTell() {
  long position = std::ftell(file_);
  if (position == -1) {
    throw ErrnoException(u"Tell failed.");
  }
  return position;
}

void CFileStream::DoRewind() { std::rewind(file_); }

Index CFileStream::DoRead(std::byte* buffer, Index offset, Index size) {
  auto count = std::fread(buffer + offset, 1, size, file_);
  return count;
}

Index CFileStream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  auto count = std::fwrite(buffer + offset, 1, size, file_);
  return count;
}

void CFileStream::DoFlush() { std::fflush(file_); }

void CFileStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE
  if (auto_close_ && !std::fclose(file_)) {
    throw Exception(u"Failed to close FILE.");
  }
  file_ = nullptr;
}
}  // namespace cru::io
