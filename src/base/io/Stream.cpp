#include "cru/base/io/Stream.h"

#include <atomic>
#include <format>
#include <ranges>
#include <utility>

namespace cru::io {
StreamException::StreamException(Stream* stream, std::string message,
                                 std::shared_ptr<std::exception> inner)
    : Exception(std::move(message), std::move(inner)), stream_(stream) {}

StreamOperationNotSupportedException::StreamOperationNotSupportedException(
    Stream* stream, std::string operation)
    : StreamException(
          stream, std::format("Stream operation {} not supported.", operation)),
      operation_(std::move(operation)) {}

void StreamOperationNotSupportedException::CheckSeek(Stream* stream,
                                                     bool seekable) {
  if (!seekable) throw StreamOperationNotSupportedException(stream, "seek");
}

void StreamOperationNotSupportedException::CheckRead(Stream* stream,
                                                     bool readable) {
  if (!readable) throw StreamOperationNotSupportedException(stream, "read");
}

void StreamOperationNotSupportedException::CheckWrite(Stream* stream,
                                                      bool writable) {
  if (!writable) throw StreamOperationNotSupportedException(stream, "write");
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
  StreamOperationNotSupportedException::CheckSeek(this, DoCanSeek());
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
  StreamOperationNotSupportedException::CheckRead(this, DoCanRead());
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
  StreamOperationNotSupportedException::CheckWrite(this, DoCanWrite());
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

bool Stream::IsClosed() { return closed_.load(std::memory_order_acquire); }

bool Stream::Close() {
  bool expected = false;
  if (closed_.compare_exchange_strong(expected, true,
                                      std::memory_order_acq_rel)) {
    DoClose();
  }
  return expected;
}

std::vector<std::byte> Stream::ReadToEnd(Index grow_size) {
  std::vector<std::byte> buffer;
  Index pos = 0;
  while (true) {
    if (pos == buffer.size()) {
      buffer.resize(buffer.size() + grow_size);
    }

    auto read = Read(buffer.data(), pos, buffer.size() - pos);
    if (read == kEOF) {
      break;
    }
    pos += read;
  }
  buffer.resize(pos);
  return buffer;
}

std::string Stream::ReadToEndAsUtf8String() {
  auto buffer = ReadToEnd();
  return std::views::transform(
             buffer, [](std::byte c) { return static_cast<char>(c); }) |
         std::ranges::to<std::string>();
}

void Stream::SetSupportedOperations(SupportedOperations supported_operations) {
  supported_operations_ = std::move(supported_operations);
}

bool Stream::DoCanSeek() {
  if (supported_operations_.can_seek) {
    return *supported_operations_.can_seek;
  } else {
    throw Exception(
        "Can seek is neither set in supported_operations nor overriden by "
        "derived class.");
  }
}

bool Stream::DoCanRead() {
  if (supported_operations_.can_read) {
    return *supported_operations_.can_read;
  } else {
    throw Exception(
        "Can read is neither set in supported_operations nor overriden by "
        "derived class.");
  }
}

bool Stream::DoCanWrite() {
  if (supported_operations_.can_write) {
    return *supported_operations_.can_write;
  } else {
    throw Exception(
        "Can write is neither set in supported_operations nor overriden by "
        "derived class.");
  }
}

Index Stream::DoSeek(Index offset, SeekOrigin origin) {
  throw Exception("Stream is seekable but DoSeek is not implemented.");
}

Index Stream::DoTell() {
  StreamOperationNotSupportedException::CheckSeek(this, DoCanSeek());
  return DoSeek(0, SeekOrigin::Current);
}

void Stream::DoRewind() {
  StreamOperationNotSupportedException::CheckSeek(this, DoCanSeek());
  DoSeek(0, SeekOrigin::Begin);
}

Index Stream::DoGetSize() {
  StreamOperationNotSupportedException::CheckSeek(this, DoCanSeek());
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

void Stream::DoClose() {}

void Stream::CheckClosed() {
  if (IsClosed()) {
    throw StreamClosedException(this, "Stream is closed.");
  }
}

}  // namespace cru::io
