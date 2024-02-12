#include "cru/common/Buffer.h"
#include <cstring>
#include "cru/common/Exception.h"

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
    size_ = used_size_ = 0;
  } else {
    ptr_ = new std::byte[size];
    size_ = size;
    used_size_ = 0;
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
                         Index src_size) {
  std::memcpy(ptr_ + dst_offset, src + src_offset, src_size);
}

void Buffer::SetUsedSize(Index new_size) { used_size_ = new_size; }

void Buffer::ResizeBuffer(Index new_size, bool preserve_used) {
  if (new_size == 0) {
    Delete_();
    ptr_ = nullptr;
    size_ = used_size_ = 0;
    return;
  }

  auto old_ptr = ptr_;

  ptr_ = new std::byte[new_size];
  size_ = new_size;

  if (old_ptr) {
    if (preserve_used) {
      auto copy_size = std::min(used_size_, new_size);
      std::memcpy(ptr_, old_ptr, copy_size);
      used_size_ = copy_size;
    }
    delete[] old_ptr;
  }
}

Index Buffer::PushEnd(std::byte* other, Index other_size) {
  auto copy_size = std::min(GetRestSize(), other_size);

  if (copy_size) {
    std::memcpy(GetUsedEndPtr(), other, copy_size);
    used_size_ += copy_size;
  }

  return copy_size;
}

Index Buffer::PopEnd(Index size) {
  if (used_size_ < size) {
    used_size_ = 0;
    return used_size_;
  } else {
    used_size_ -= size;
    return size;
  }
}

void Buffer::Copy_(const Buffer& other) {
  if (other.ptr_ == nullptr) {
    ptr_ = nullptr;
    size_ = used_size_ = 0;
  } else {
    ptr_ = new std::byte[other.size_];
    size_ = other.size_;
    used_size_ = other.used_size_;
    std::memcpy(ptr_, other.ptr_, used_size_);
  }
}

void Buffer::Move_(Buffer&& other) noexcept {
  ptr_ = other.ptr_;
  size_ = other.size_;
  used_size_ = other.used_size_;
  other.ptr_ = nullptr;
  other.size_ = other.used_size_ = 0;
}

void Buffer::Delete_() noexcept {
  if (ptr_) {
    delete[] ptr_;
  }
}
}  // namespace cru
