#pragma once

#include "../Base.hpp"

#include <cstddef>

namespace cru::io {
class CRU_BASE_API Stream : public Object {
 public:
  enum class SeekOrigin { Current, Begin, End };

  Stream() = default;

  CRU_DELETE_COPY(Stream)
  CRU_DELETE_MOVE(Stream)

  ~Stream() override = default;

 public:
  virtual bool CanSeek() = 0;
  virtual Index Tell() = 0;
  virtual void Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) = 0;
  virtual void Rewind();
  virtual Index GetSize();

  virtual bool CanRead() = 0;
  virtual Index Read(std::byte* buffer, Index offset, Index size) = 0;
  virtual Index Read(std::byte* buffer, Index size);

  virtual bool CanWrite() = 0;
  virtual Index Write(const std::byte* buffer, Index offset, Index size) = 0;
  virtual Index Write(const std::byte* buffer, Index size);

  virtual void Flush() = 0;
};
}  // namespace cru::io
