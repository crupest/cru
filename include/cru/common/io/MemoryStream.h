#pragma once

#include "Stream.h"

#include <functional>

namespace cru::io {
class CRU_BASE_API MemoryStream : public Stream {
 public:
  MemoryStream(
      std::byte* buffer, Index size, bool read_only = false,
      std::function<void(std::byte* buffer, Index size)> release_func = {});

  ~MemoryStream() override;

 public:
  void Close() override;

  std::byte* GetBuffer() const { return buffer_; }

 protected:
  Index DoSeek(Index offset, SeekOrigin origin) override;
  Index DoGetSize() override { return size_; }
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;

 private:
  void DoClose();

 private:
  std::byte* buffer_;
  Index size_;
  Index position_;
  std::function<void(std::byte* buffer, Index size)> release_func_;
};
}  // namespace cru::io
