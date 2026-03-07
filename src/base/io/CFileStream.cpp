#include "cru/base/io/CFileStream.h"
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
    throw StreamIOException(this, "fopen failed.",
                            std::make_shared<ErrnoException>());
  }
}

CFileStream::CFileStream(std::FILE* file, bool readable, bool writable,
                         bool auto_close)
    : Stream(true, readable, writable), file_(file), auto_close_(auto_close) {
  if (file_ == nullptr) {
    throw Exception("File is NULL.");
  }
}

CFileStream::~CFileStream() {
  if (file_ && auto_close_) {
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
      throw Exception("Unknown seek origin.");
  }
}

Index CFileStream::DoSeek(Index offset, SeekOrigin origin) {
  if (std::fseek(file_, offset, ConvertOriginFlag(origin))) {
    throw StreamIOException(this, "fseek failed.",
                            std::make_shared<ErrnoException>());
  }
  return DoTell();
}

Index CFileStream::DoTell() {
  long position = std::ftell(file_);
  if (position == -1) {
    throw StreamIOException(this, "ftell failed.",
                            std::make_shared<ErrnoException>());
  }
  return position;
}

void CFileStream::DoRewind() { std::rewind(file_); }

Index CFileStream::DoRead(std::byte* buffer, Index offset, Index size) {
  auto count = std::fread(buffer + offset, 1, size, file_);
  if (std::ferror(file_)) {
    throw StreamIOException(this, "Error occurred when reading C FILE.");
  }
  if (count == 0 && std::feof(file_)) {
    return kEOF;
  }
  return count;
}

Index CFileStream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  auto count = std::fwrite(buffer + offset, 1, size, file_);
  if (std::ferror(file_)) {
    throw StreamIOException(this, "Error occurred when writing C FILE.");
  }
  return count;
}

void CFileStream::DoFlush() { std::fflush(file_); }

void CFileStream::DoClose() {
  if (auto_close_ && !std::fclose(file_)) {
    throw StreamIOException(this, "fclose failed.");
  }
  file_ = nullptr;
}
}  // namespace cru::io
