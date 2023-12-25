#include "cru/common/io/ProxyStream.h"
#include "cru/common/io/Stream.h"

namespace cru::io {
ProxyStream::ProxyStream(ProxyStreamHandlers handlers)
    : closed_(false), handlers_(std::move(handlers)) {}

ProxyStream::~ProxyStream() { DoClose(); }

void ProxyStream::DoClose() {
  if (!closed_) {
    if (handlers_.close) {
      handlers_.close();
    }
    closed_ = true;
  }
}
}  // namespace cru::io
