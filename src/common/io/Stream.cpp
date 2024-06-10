#include "cru/common/io/Stream.h"
#include "cru/common/Exception.h"
#include "cru/common/Format.h"

#include <utility>

namespace cru::io {
StreamOperationNotSupportedException::StreamOperationNotSupportedException(
    String operation)
    : operation_(std::move(operation)) {
  SetMessage(Format(u"Stream operation {} not supported.", operation_));
}

void StreamOperationNotSupportedException::CheckSeek(bool seekable) {
  if (!seekable) throw StreamOperationNotSupportedException(u"seek");
}

void StreamOperationNotSupportedException::CheckRead(bool readable) {
  if (!readable) throw StreamOperationNotSupportedException(u"read");
}

void StreamOperationNotSupportedException::CheckWrite(bool writable) {
  if (!writable) throw StreamOperationNotSupportedException(u"write");
}

StreamAlreadyClosedException::StreamAlreadyClosedException()
    : Exception(u"Stream is already closed.") {}

void StreamAlreadyClosedException::Check(bool closed) {
  if (closed) throw StreamAlreadyClosedException();
}

Index Stream::Tell() { return Seek(0, SeekOrigin::Current); }

void Stream::Rewind() { Seek(0, SeekOrigin::Begin); }

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

Buffer Stream::ReadToEnd(Index grow_size) {
  Buffer buffer(grow_size);
  while (true) {
    auto read = Read(buffer.GetUsedEndPtr(), buffer.GetBackFree());
    if (read == 0) {
      break;
    }
    if (buffer.IsUsedReachEnd()) {
      buffer.ResizeBuffer(buffer.GetBufferSize() + grow_size, true);
    }
  }
  return buffer;
}

String Stream::ReadToEndAsUtf8String() {
  auto buffer = ReadToEnd();
  return String::FromUtf8(buffer);
}

void Stream::Flush() {}

void Stream::Close() {}
}  // namespace cru::io
