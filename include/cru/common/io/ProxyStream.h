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

  CRU_DELETE_COPY(ProxyStream)
  CRU_DELETE_MOVE(ProxyStream)

  ~ProxyStream() override;

 public:
  bool CanSeek() override;
  Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;

  bool CanRead() override;
  Index Read(std::byte* buffer, Index offset, Index size) override;

  bool CanWrite() override;
  Index Write(const std::byte* buffer, Index offset, Index size) override;

  void Flush() override;

  void Close() override;

 private:
  void DoClose();

 private:
  bool closed_;
  ProxyStreamHandlers handlers_;
};
}  // namespace cru::io
