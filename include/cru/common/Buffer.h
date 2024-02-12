#pragma once

#include "Base.h"

namespace cru {
class Buffer {
 public:
  explicit Buffer(Index size);

  Buffer(const Buffer& other);
  Buffer(Buffer&& other) noexcept;

  Buffer& operator=(const Buffer& other);
  Buffer& operator=(Buffer&& other) noexcept;

  ~Buffer();

 private:
  Index GetSize() const;
  Index GetUsedSize() const;
  bool IsFull() const { return GetSize() == GetUsedSize(); }
  bool IsNotFull() const { return !IsFull(); }

  std::byte& GetAt(Index index);
  std::byte GetAt(Index index) const;

  Index PushEnd(std::byte* other, Index other_size);
  Index PopEnd(Index size);

  std::byte* Data();
  const std::byte* Data() const;

  operator std::byte*() { return Data(); }
  operator const std::byte*() const { return Data(); }

 private:
  std::byte* ptr_;
  Index size_;
  Index used_size_;
};

void swap(Buffer& left, Buffer& right);

class BufferList {
  public:
  explicit BufferList(Index buffer_size);
  private:

};
}  // namespace cru
