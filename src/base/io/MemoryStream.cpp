#include "cru/base/io/MemoryStream.h"

#include <cstring>
#include "cru/base/io/Stream.h"

namespace cru::io {
MemoryStream::MemoryStream(
    std::byte* buffer, Index size, bool read_only,
    std::function<void(std::byte* buffer, Index size)> release_func)
    : Stream(true, true, !read_only),
      buffer_(buffer),
      size_(size),
      position_(0),
      release_func_(std::move(release_func)) {
  if (!buffer) {
    throw Exception("Buffer is nullptr");
  }
  if (size <= 0) {
    throw Exception("Size is 0 or negative.");
  }
}

MemoryStream::~MemoryStream() {}

void MemoryStream::Close() { DoClose(); }

Index MemoryStream::DoSeek(Index offset, SeekOrigin origin) {
  switch (origin) {
    case SeekOrigin::Current:
      position_ += offset;
      break;
    case SeekOrigin::Begin:
      position_ = offset;
      break;
    case SeekOrigin::End:
      position_ = size_ + offset;
      break;
  }
  return position_;
}

Index MemoryStream::DoRead(std::byte* buffer, Index offset, Index size) {
  if (position_ + size > size_) {
    size = size_ - position_;
  }
  if (size <= 0) {
    return 0;
  }
  std::memmove(buffer + offset, buffer_ + position_, size);
  position_ += size;
  return size;
}

Index MemoryStream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  if (position_ + size > size_) {
    size = size_ - position_;
  }
  if (size <= 0) {
    return 0;
  }
  std::memmove(buffer_ + position_, buffer + offset, size);
  position_ += size;
  return size;
}

void MemoryStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE
  release_func_(buffer_, size_);
  buffer_ = nullptr;
  release_func_ = {};
}

}  // namespace cru::io
