#include "cru/common/io/ProxyStream.h"
#include "cru/common/io/Stream.h"

namespace cru::io {
ProxyStream::ProxyStream(ProxyStreamHandlers handlers)
    : closed_(false), handlers_(std::move(handlers)) {}

ProxyStream::~ProxyStream() { DoClose(); }

bool ProxyStream::CanSeek() {
  CheckClosed();
  return static_cast<bool>(handlers_.seek);
}

Index ProxyStream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckSeek(CanSeek());
  return handlers_.seek(offset, origin);
}

bool ProxyStream::CanRead() {
  CheckClosed();
  return static_cast<bool>(handlers_.read);
}

Index ProxyStream::Read(std::byte* buffer, Index offset, Index size) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckRead(CanRead());
  return handlers_.read(buffer, offset, size);
}

bool ProxyStream::CanWrite() {
  CheckClosed();
  return static_cast<bool>(handlers_.write);
}

Index ProxyStream::Write(const std::byte* buffer, Index offset, Index size) {
  CheckClosed();
  StreamOperationNotSupportedException::CheckWrite(CanWrite());
  return handlers_.write(buffer, offset, size);
}

void ProxyStream::Flush() {
  CheckClosed();
  if (handlers_.flush) {
    handlers_.flush();
  }
}

void ProxyStream::Close() { DoClose(); }

void ProxyStream::CheckClosed() {
  StreamAlreadyClosedException::Check(closed_);
}

void ProxyStream::DoClose() {
  if (!closed_) {
    if (handlers_.close) {
      handlers_.close();
    }
    closed_ = true;
  }
}
}  // namespace cru::io
