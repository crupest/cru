#pragma once

#include "Stream.hpp"

namespace cru::io {
class CRU_BASE_API MemoryStream : public Stream {
 public:
  MemoryStream() = default;
  MemoryStream(std::byte* buffer, Index size, bool read_only = false,
               bool auto_release = false)
      : buffer_(buffer),
        size_(size),
        read_only_(read_only),
        auto_release_(auto_release) {}

  CRU_DELETE_COPY(MemoryStream)
  CRU_DELETE_MOVE(MemoryStream)

  ~MemoryStream() override;

 public:
  bool CanSeek() override;
  Index Tell() override;
  void Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;

  bool CanRead() override;
  Index Read(std::byte* buffer, Index offset, Index size) override;

  bool CanWrite() override;
  Index Write(const std::byte* buffer, Index offset, Index size) override;

 private:
  std::byte* buffer_ = nullptr;
  Index size_ = 0;
  Index position_ = 0;
  bool auto_release_ = false;
  bool read_only_ = false;
};
}  // namespace cru::io
