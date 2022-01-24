#include "cru/common/io/Stream.hpp"

namespace cru::io {
void Stream::Rewind() { Seek(0); }

Index Stream::GetSize() {
  Index current_position = Tell();
  Seek(0, SeekOrigin::End);
  Index size = Tell();
  Seek(current_position, SeekOrigin::Begin);
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

std::vector<std::byte> Stream::ReadAll() {
  std::vector<std::byte> buffer;
  buffer.resize(GetSize());
  Read(buffer.data(), 0, buffer.size());
  return buffer;
}

String Stream::ReadAllAsString() {
  auto buffer = ReadAll();
  return String::FromUtf8(reinterpret_cast<const char*>(buffer.data()),
                          buffer.size());
}

void Stream::Flush() {}

void Stream::Close() {}
}  // namespace cru::io
