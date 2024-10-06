#pragma once

#include "Stream.h"

#include <functional>

namespace cru::io {
struct ProxyStreamHandlers {
  std::function<Index(Index offset, Stream::SeekOrigin origin)> seek;
  std::function<Index(std::byte* buffer, Index offset, Index size)> read;
  std::function<Index(const std::byte* buffer, Index offset, Index size)> write;
  std::function<void()> flush;

  /**
   * @brief This method will be only called once when `Close` is called or the
   * stream is destructed.
   */
  std::function<void()> close;
};

class ProxyStream : public Stream {
 public:
  explicit ProxyStream(ProxyStreamHandlers handlers);

  ~ProxyStream() override;

 public:
  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

 protected:
  Index DoSeek(Index offset, SeekOrigin origin) override;
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;
  void DoFlush() override;

 private:
  void DoClose();

 private:
  ProxyStreamHandlers handlers_;
};
}  // namespace cru::io
