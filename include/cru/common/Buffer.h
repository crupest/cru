#pragma once

#include "Base.h"

namespace cru {
class Buffer final {
  friend void swap(Buffer& left, Buffer& right) noexcept;

 public:
  explicit Buffer(Index size);

  Buffer(const Buffer& other);
  Buffer(Buffer&& other) noexcept;

  Buffer& operator=(const Buffer& other);
  Buffer& operator=(Buffer&& other) noexcept;

  ~Buffer();

 public:
  Index GetBufferSize() const { return size_; }
  Index GetUsedSize() const { return used_end_ - used_begin_; }
  bool IsNull() const { return ptr_ == nullptr; }
  bool IsUsedReachEnd() const { return used_end_ == size_; }

  Index GetFrontFree() const { return used_begin_; }
  Index GetBackFree() const { return size_ - used_end_; }

  Index GetUsedBegin() const { return used_begin_; }
  Index GetUsedEnd() const { return used_end_; }

  std::byte* GetPtr() { return GetPtrAt(0); }
  const std::byte* GetPtr() const { return GetPtrAt(0); }

  std::byte* GetPtrAt(Index index) { return ptr_ + index; }
  const std::byte* GetPtrAt(Index index) const { return ptr_ + index; }

  std::byte& GetRefAt(Index index) { return *GetPtrAt(index); }
  const std::byte& GetRefAt(Index index) const { return *GetPtrAt(index); }

  std::byte* GetUsedBeginPtr() { return GetPtrAt(GetUsedBegin()); }
  const std::byte* GetUsedBeginPtr() const { return GetPtrAt(GetUsedBegin()); }
  std::byte* GetUsedEndPtr() { return GetPtrAt(GetUsedEnd()); }
  const std::byte* GetUsedEndPtr() const { return GetPtrAt(GetUsedEnd()); }

  std::byte GetByteAt(Index index) const { return ptr_[index]; }
  void SetByteAt(Index index, std::byte value) { ptr_[index] = value; }

  void AssignBytes(std::byte* src, Index src_size, bool use_memmove = false) {
    return AssignBytes(0, src, 0, src_size, use_memmove);
  }
  void AssignBytes(Index dst_offset, std::byte* src, Index src_size,
                   bool use_memmove = false) {
    return AssignBytes(dst_offset, src, 0, src_size, use_memmove);
  }
  void AssignBytes(Index dst_offset, std::byte* src, Index src_offset,
                   Index src_size, bool use_memmove = false);

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
   * @brief Append data to the front of used bytes and increase used size.
   * @return The actual size of data saved.
   *
   * If there is no enough space left for new data, the rest space will be
   * written and the size of it will be returned, leaving exceeded data not
   * saved.
   */
  Index PushFront(const std::byte* other, Index other_size,
                  bool use_memmove = false);

  bool PushBack(std::byte b);

  /**
   * @brief Append data to the back of used bytes and increase used size.
   * @return The actual size of data saved.
   *
   * If there is no enough space left for new data, the rest space will be
   * written and the size of it will be returned, leaving exceeded data not
   * saved.
   */
  Index PushBack(const std::byte* other, Index other_size,
                 bool use_memmove = false);

  /**
   * @brief Move forward the used-begin ptr.
   * @return The actual size moved forward.
   *
   * If given size is bigger than current used size, the used size will be
   * returned and set to 0.
   */
  Index PopFront(Index size);

  /**
   * @brief Pop front data of used bytes into another buffer.
   * @return The actual size popped.
   *
   * If given size is bigger than current used size, then only current used size
   * of bytes will be popped. If given size is smaller than current used size,
   * then only given size of bytes will be popped.
   */
  Index PopFront(std::byte* buffer, Index size, bool use_memmove = false);

  /**
   * @brief Move backward the used-end ptr.
   * @return The actual size moved backward.
   *
   * If given size is bigger than current used size, the used size will be
   * returned and set to 0.
   */
  Index PopEnd(Index size);

  /**
   * @brief Pop back data of used bytes into another buffer.
   * @return The actual size popped.
   *
   * If given size is bigger than current used size, then only current used size
   * of bytes will be popped. If given size is smaller than current used size,
   * then only given size of bytes will be popped.
   */
  Index PopEnd(std::byte* buffer, Index size, bool use_memmove = false);

  operator std::byte*() { return GetPtr(); }
  operator const std::byte*() const { return GetPtr(); }

  /**
   * @brief Detach internal buffer and return it.
   * @param size If not null, size of the buffer is written to it.
   * @return The buffer pointer. May be nullptr.
   *
   * After detach, you are responsible to delete[] it.
   */
  std::byte* Detach(Index* size = nullptr);

 private:
  void Copy_(const Buffer& other);
  void Move_(Buffer&& other) noexcept;
  void Delete_() noexcept;

 private:
  std::byte* ptr_;
  Index size_;
  Index used_begin_;
  Index used_end_;
};

void swap(Buffer& left, Buffer& right) noexcept;
}  // namespace cru
