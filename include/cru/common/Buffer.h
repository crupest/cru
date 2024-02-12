#pragma once

#include "Base.h"

#include <list>

namespace cru {
class Buffer final {
 public:
  explicit Buffer(Index size);

  Buffer(const Buffer& other);
  Buffer(Buffer&& other) noexcept;

  Buffer& operator=(const Buffer& other);
  Buffer& operator=(Buffer&& other) noexcept;

  ~Buffer();

 private:
  Index GetBufferSize() const { return size_; }
  Index GetUsedSize() const { return used_size_; }
  Index GetRestSize() const { return GetBufferSize() - GetUsedSize(); }
  bool IsNull() const { return ptr_ == nullptr; }
  bool IsFull() const { return GetBufferSize() == GetUsedSize(); }

  std::byte* GetPtr() { return GetPtrAt(0); }
  const std::byte* GetPtr() const { return GetPtrAt(0); }

  std::byte* GetPtrAt(Index index) { return ptr_ + index; }
  const std::byte* GetPtrAt(Index index) const { return ptr_ + index; }

  std::byte& GetRefAt(Index index) { return *GetPtrAt(index); }
  const std::byte& GetRefAt(Index index) const { return *GetPtrAt(index); }

  std::byte* GetUsedEndPtr() { return GetPtrAt(GetUsedSize()); }
  const std::byte* GetUsedEndPtr() const { return GetPtrAt(GetUsedSize()); }

  std::byte GetByteAt(Index index) const { return ptr_[index]; }
  void SetByteAt(Index index, std::byte value) { ptr_[index] = value; }

  void AssignBytes(std::byte* src, Index src_size) {
    return AssignBytes(0, src, 0, src_size);
  }
  void AssignBytes(Index dst_offset, std::byte* src, Index src_size) {
    return AssignBytes(dst_offset, src, 0, src_size);
  }
  void AssignBytes(Index dst_offset, std::byte* src, Index src_offset,
                   Index src_size);

  void SetUsedSize(Index new_size);

  /**
   * @brief Change the size of the buffer.
   *
   * Unless new size is the same as current size, the buffer is always released
   * and a new one is allocated. If preserve_used is true, the used size and old
   * data is copied to the new buffer. If new size is smaller than old used
   * size, then exceeded data will be lost.
   */
  void ResizeBuffer(Index new_size, bool preserve_used);

  /**
   * @brief Append data to the back of used bytes and increase used size.
   * @return The actual size of data saved.
   *
   * If there is no enough space left for new data, the rest space will be
   * written and the size of it will be returned, leaving exceeded data not
   * saved.
   */
  Index PushEnd(std::byte* other, Index other_size);

  /**
   * @brief Decrease used data size.
   * @return The actual size decreased.
   *
   * If given size is bigger than current used size, the used size will be
   * returned and set to 0.
   */
  Index PopEnd(Index size);

  operator std::byte*() { return GetPtr(); }
  operator const std::byte*() const { return GetPtr(); }

 private:
  void Copy_(const Buffer& other);
  void Move_(Buffer&& other) noexcept;
  void Delete_() noexcept;

 private:
  std::byte* ptr_;
  Index size_;
  Index used_size_;
};

void swap(Buffer& left, Buffer& right);

class BufferList {
 public:
  explicit BufferList(Index buffer_size);

  BufferList(const BufferList& other);
  BufferList(BufferList&& other);

  BufferList& operator=(const BufferList& other);
  BufferList& operator=(BufferList&& other);

  ~BufferList();

 private:
  std::list<Buffer> buffers_;
};
}  // namespace cru
