#include "cru/common/io/ProxyStream.h"
#include "cru/common/io/Stream.h"

namespace cru::io {
ProxyStream::ProxyStream(ProxyStreamHandlers handlers)
    : Stream(static_cast<bool>(handlers.seek), static_cast<bool>(handlers.read),
             static_cast<bool>(handlers.write)),
      handlers_(std::move(handlers)) {}

ProxyStream::~ProxyStream() { DoClose(); }

Index ProxyStream::DoSeek(Index offset, SeekOrigin origin) {
  return handlers_.seek(offset, origin);
}

Index ProxyStream::DoRead(std::byte* buffer, Index offset, Index size) {
  return handlers_.read(buffer, offset, size);
}

Index ProxyStream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  return handlers_.write(buffer, offset, size);
}

void ProxyStream::DoFlush() {
  if (handlers_.flush) {
    handlers_.flush();
  }
}

void ProxyStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE
  if (handlers_.close) {
    handlers_.close();
  }
  handlers_ = {};
}
}  // namespace cru::io
