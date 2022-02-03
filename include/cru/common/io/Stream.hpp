#pragma once

#include "../Base.hpp"

#include "../String.hpp"

#include <cstddef>
#include <vector>

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
  virtual Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) = 0;
  virtual Index Tell();
  virtual void Rewind();
  virtual Index GetSize();

  virtual bool CanRead() = 0;
  virtual Index Read(std::byte* buffer, Index offset, Index size) = 0;
  virtual Index Read(std::byte* buffer, Index size);

  virtual bool CanWrite() = 0;
  virtual Index Write(const std::byte* buffer, Index offset, Index size) = 0;
  virtual Index Write(const std::byte* buffer, Index size);
  Index Write(const char* buffer, Index offset, Index size);
  Index Write(const char* buffer, Index size);

  virtual std::vector<std::byte> ReadAll();

  // Utf8 encoding.
  String ReadAllAsString();

  virtual void Flush();

  virtual void Close();
};
}  // namespace cru::io
