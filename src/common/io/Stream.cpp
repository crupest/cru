#include "cru/common/io/Stream.hpp"

namespace cru::io {
void Stream::Rewind() { Seek(0); }

Index Stream::GetSize() {
  Index current_position = Tell();
  Seek(0, SeekOrigin::End);
  Index size = Tell();
  Seek(current_position);
  return size;
}

Index Stream::Read(std::byte* buffer, Index size) {
  return Read(buffer, 0, size);
}

Index Stream::Write(const std::byte* buffer, Index size) {
  return Write(buffer, 0, size);
}

Index Stream::Write(const char* buffer, Index offset, Index size) {
  return Write(reinterpret_cast<const std::byte*>(buffer), offset, size);
}

Index Stream::Write(const char* buffer, Index size) {
  return Write(reinterpret_cast<const std::byte*>(buffer), size);
}

void Stream::Flush() {}

void Stream::Close() {}
}  // namespace cru::io
