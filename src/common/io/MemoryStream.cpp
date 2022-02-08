#include "cru/common/io/MemoryStream.h"

namespace cru::io {
MemoryStream::~MemoryStream() {
  if (release_func_) {
    release_func_(buffer_, size_);
  }
}

bool MemoryStream::CanSeek() { return true; }

Index MemoryStream::Seek(Index offset, SeekOrigin origin) {
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

bool MemoryStream::CanRead() { return true; }

Index MemoryStream::Read(std::byte *buffer, Index offset, Index size) {
  if (position_ + size > size_) {
    size = size_ - position_;
  }
  if (size <= 0) {
    return 0;
  }
  std::memcpy(buffer + offset, buffer_ + position_, size);
  position_ += size;
  return size;
}

bool MemoryStream::CanWrite() { return !read_only_; }

Index MemoryStream::Write(const std::byte *buffer, Index offset, Index size) {
  if (read_only_) {
    return 0;
  }
  if (position_ + size > size_) {
    size = size_ - position_;
  }
  if (size <= 0) {
    return 0;
  }
  std::memcpy(buffer_ + position_, buffer + offset, size);
  position_ += size;
  return size;
}
}  // namespace cru::io
