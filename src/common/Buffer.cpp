#include "cru/common/Buffer.h"
#include "cru/common/Exception.h"

#include <cstring>

namespace cru {
namespace {
void CheckSize(Index size) {
  if (size < 0) {
    throw Exception(u"Size of buffer can't be smaller than 0.");
  }
}
}  // namespace

Buffer::Buffer(Index size) {
  CheckSize(size);
  if (size == 0) {
    ptr_ = nullptr;
    size_ = used_begin_ = used_end_ = 0;
  } else {
    ptr_ = new std::byte[size];
    size_ = size;
    used_begin_ = used_end_ = 0;
  }
}

Buffer::Buffer(const Buffer& other) { Copy_(other); }

Buffer::Buffer(Buffer&& other) noexcept { Move_(std::move(other)); }

Buffer& Buffer::operator=(const Buffer& other) {
  if (this != &other) {
    Delete_();
    Copy_(other);
  }
  return *this;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
  if (this != &other) {
    Delete_();
    Move_(std::move(other));
  }
  return *this;
}

Buffer::~Buffer() { Delete_(); }

void Buffer::AssignBytes(Index dst_offset, std::byte* src, Index src_offset,
                         Index src_size, bool use_memmove) {
  (use_memmove ? std::memmove : std::memcpy)(ptr_ + dst_offset,
                                             src + src_offset, src_size);
}

void Buffer::ResizeBuffer(Index new_size, bool preserve_used) {
  if (new_size == 0) {
    Delete_();
    ptr_ = nullptr;
    size_ = used_begin_ = used_end_ = 0;
    return;
  }

  auto old_ptr = ptr_;

  ptr_ = new std::byte[new_size];
  size_ = new_size;
  used_begin_ = std::min(new_size, used_begin_);
  used_end_ = std::min(new_size, used_end_);

  if (old_ptr) {
    if (preserve_used && used_begin_ < used_end_) {
      std::memcpy(ptr_ + used_begin_, old_ptr + used_begin_,
                  used_end_ - used_begin_);
    }
    delete[] old_ptr;
  }
}

Index Buffer::PushFront(const std::byte* other, Index other_size,
                        bool use_memmove) {
  auto copy_size = std::min(used_begin_, other_size);

  if (copy_size) {
    used_begin_ -= copy_size;
    (use_memmove ? std::memmove : std::memcpy)(ptr_ + used_begin_, other,
                                               copy_size);
  }

  return copy_size;
}

bool Buffer::PushBack(std::byte b) {
  if (IsUsedReachEnd()) {
    return false;
  }
  ptr_[used_end_] = b;
  used_end_++;
  return true;
}

Index Buffer::PushBack(const std::byte* other, Index other_size,
                       bool use_memmove) {
  auto copy_size = std::min(size_ - used_end_, other_size);

  if (copy_size) {
    (use_memmove ? std::memmove : std::memcpy)(ptr_ + used_end_, other,
                                               copy_size);
    used_end_ += copy_size;
  }

  return copy_size;
}

Index Buffer::PopFront(Index size) {
  auto move = std::min(used_begin_, size);
  used_begin_ -= move;
  return move;
}

Index Buffer::PopFront(std::byte* buffer, Index size, bool use_memmove) {
  auto pop_size = std::min(GetUsedSize(), size);

  if (pop_size) {
    used_begin_ += pop_size;
    (use_memmove ? std::memmove : std::memcpy)(
        buffer, GetUsedBeginPtr() - pop_size, pop_size);
  }

  return pop_size;
}

Index Buffer::PopEnd(Index size) {
  auto move = std::min(size_ - used_end_, size);
  used_end_ += move;
  return move;
}

Index Buffer::PopEnd(std::byte* buffer, Index size, bool use_memmove) {
  auto pop_size = std::min(GetUsedSize(), size);

  if (pop_size) {
    used_end_ -= pop_size;
    (use_memmove ? std::memmove : std::memcpy)(buffer, GetUsedEndPtr(),
                                               pop_size);
  }

  return pop_size;
}

std::byte* Buffer::Detach(Index* size) {
  auto ptr = this->ptr_;
  if (size) {
    *size = this->size_;
  }
  this->ptr_ = nullptr;
  this->size_ = this->used_begin_ = this->used_end_ = 0;
  return ptr;
}

void Buffer::Copy_(const Buffer& other) {
  if (other.ptr_ == nullptr) {
    ptr_ = nullptr;
    size_ = used_begin_ = used_end_ = 0;
  } else {
    ptr_ = new std::byte[other.size_];
    size_ = other.size_;
    used_begin_ = other.used_begin_;
    used_end_ = other.used_end_;
    std::memcpy(ptr_ + used_begin_, other.ptr_ + used_begin_,
                used_end_ - used_begin_);
  }
}

void Buffer::Move_(Buffer&& other) noexcept {
  ptr_ = other.ptr_;
  size_ = other.size_;
  used_begin_ = other.used_begin_;
  used_end_ = other.used_end_;
  other.ptr_ = nullptr;
  other.size_ = other.used_begin_ = other.used_end_ = 0;
}

void Buffer::Delete_() noexcept {
  if (ptr_) {
    delete[] ptr_;
  }
}

void swap(Buffer& left, Buffer& right) noexcept {
  using std::swap;
  swap(left.ptr_, right.ptr_);
  swap(left.size_, right.size_);
  swap(left.used_begin_, right.used_begin_);
  swap(left.used_end_, right.used_end_);
}
}  // namespace cru
