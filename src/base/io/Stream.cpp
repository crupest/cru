#include "cru/base/io/Stream.h"
#include "cru/base/Exception.h"

#include <format>
#include <utility>

namespace cru::io {
StreamOperationNotSupportedException::StreamOperationNotSupportedException(
    std::string operation)
    : Exception(std::format("Stream operation {} not supported.", operation)),
      operation_(std::move(operation)) {}

void StreamOperationNotSupportedException::CheckSeek(bool seekable) {
  if (!seekable) throw StreamOperationNotSupportedException("seek");
}

void StreamOperationNotSupportedException::CheckRead(bool readable) {
  if (!readable) throw StreamOperationNotSupportedException("read");
}

void StreamOperationNotSupportedException::CheckWrite(bool writable) {
  if (!writable) throw StreamOperationNotSupportedException("write");
}

StreamClosedException::StreamClosedException()
    : Exception("Stream is already closed.") {}

void StreamClosedException::Check(bool closed) {
  if (closed) throw StreamClosedException();
}

Stream::Stream(SupportedOperations supported_operations)
    : supported_operations_(std::move(supported_operations)), closed_(false) {}

Stream::Stream(std::optional<bool> can_seek, std::optional<bool> can_read,
               std::optional<bool> can_write)
    : Stream(SupportedOperations{can_seek, can_read, can_write}) {}

bool Stream::CanSeek() {
  CheckClosed();
  return DoCanSeek();
}

Index Stream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckSeek(DoCanSeek());
  return DoSeek(offset, origin);
}

Index Stream::Tell() {
  CheckClosed();
  return DoTell();
}

void Stream::Rewind() {
  CheckClosed();
  return DoRewind();
}

Index Stream::GetSize() {
  CheckClosed();
  return DoGetSize();
}

bool Stream::CanRead() {
  CheckClosed();
  return DoCanRead();
}

Index Stream::Read(std::byte* buffer, Index offset, Index size) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckRead(DoCanRead());
  return DoRead(buffer, offset, size);
}

Index Stream::Read(std::byte* buffer, Index size) {
  return Read(buffer, 0, size);
}

Index Stream::Read(char* buffer, Index offset, Index size) {
  return Read(reinterpret_cast<std::byte*>(buffer), offset, size);
}

Index Stream::Read(char* buffer, Index size) {
  return Read(reinterpret_cast<std::byte*>(buffer), 0, size);
}

bool Stream::CanWrite() {
  CheckClosed();
  return DoCanWrite();
}

Index Stream::Write(const std::byte* buffer, Index offset, Index size) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckWrite(DoCanWrite());
  return DoWrite(buffer, offset, size);
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

void Stream::Flush() {
  CheckClosed();
  DoFlush();
}

bool Stream::DoCanSeek() {
  if (supported_operations_->can_seek) {
    return *supported_operations_->can_seek;
  } else {
    throw Exception(
        "Can seek is neither set in supported_operations nor implemeted in "
        "virtual function.");
  }
}

bool Stream::DoCanRead() {
  if (supported_operations_->can_read) {
    return *supported_operations_->can_read;
  } else {
    throw Exception(
        "Can read is neither set in supported_operations nor implemeted in "
        "virtual function.");
  }
}

bool Stream::DoCanWrite() {
  if (supported_operations_->can_write) {
    return *supported_operations_->can_write;
  } else {
    throw Exception(
        "Can write is neither set in supported_operations nor implemeted in "
        "virtual function.");
  }
}

Index Stream::DoSeek(Index offset, SeekOrigin origin) {
  throw Exception("Stream is seekable but DoSeek is not implemented.");
}

Index Stream::DoTell() {
  StreamOperationNotSupportedException::CheckSeek(DoCanSeek());
  return DoSeek(0, SeekOrigin::Current);
}

void Stream::DoRewind() {
  StreamOperationNotSupportedException::CheckSeek(DoCanSeek());
  DoSeek(0, SeekOrigin::Begin);
}

Index Stream::DoGetSize() {
  StreamOperationNotSupportedException::CheckSeek(DoCanSeek());
  Index current_position = DoTell();
  Seek(0, SeekOrigin::End);
  Index size = DoTell();
  Seek(current_position, SeekOrigin::Begin);
  return size;
}

Index Stream::DoRead(std::byte* buffer, Index offset, Index size) {
  throw Exception("Stream is readable but DoRead is not implemented.");
}

Index Stream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  throw Exception("Stream is writable but DoWrite is not implemented.");
}

void Stream::DoFlush() {}

Buffer Stream::ReadToEnd(Index grow_size) {
  Buffer buffer(grow_size);
  while (true) {
    auto read = Read(buffer.GetUsedEndPtr(), buffer.GetBackFree());
    buffer.PushBackCount(read);
    if (read == 0) {
      break;
    }
    if (buffer.IsUsedReachEnd()) {
      buffer.ResizeBuffer(buffer.GetBufferSize() + grow_size, true);
    }
  }
  return buffer;
}

std::string Stream::ReadToEndAsUtf8String() {
  auto buffer = ReadToEnd();
  return std::string(buffer.GetUsedBeginPtr(), buffer.GetUsedEndPtr());
}
}  // namespace cru::io
