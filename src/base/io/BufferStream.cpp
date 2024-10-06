#include "cru/base/io/BufferStream.h"
#include "cru/base/io/Stream.h"

namespace cru::io {
BufferStream::BufferStream(const BufferStreamOptions& options)
    : Stream(false, true, true) {
  block_size_ = options.GetBlockSizeOrDefault();
  max_block_count_ = options.GetMaxBlockCount();

  eof_ = false;
}

BufferStream::~BufferStream() { DoClose(); }

Index BufferStream::DoRead(std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(mutex_);

  condition_variable_.wait(
      lock, [this] { return GetClosed() || !buffer_list_.empty() || eof_; });

  if (GetClosed()) {
    StreamClosedException::Check(true);
  }

  if (buffer_list_.empty() && eof_) {
    return 0;
  }

  auto full = max_block_count_ > 0 && buffer_list_.size() == max_block_count_;

  Index read = 0;

  while (!buffer_list_.empty()) {
    auto& stream_buffer = buffer_list_.front();
    auto this_read =
        stream_buffer.PopFront(buffer + offset + read, size - read);
    if (stream_buffer.GetUsedSize() == 0) {
      buffer_list_.pop_front();
    }
    read += this_read;
    if (read == size) {
      break;
    }
  }

  if (full && buffer_list_.size() < max_block_count_) {
    // By convention, there should be at most one producer waiting. So
    // notify_one and notify_all should be the same.
    condition_variable_.notify_one();
  }

  return read;
}

Index BufferStream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(mutex_);

  if (eof_) {
    throw WriteAfterEofException(
        u"Stream has been set eof. Can't write to it any more.");
  }

  condition_variable_.wait(lock, [this] {
    return GetClosed() || max_block_count_ <= 0 ||
           buffer_list_.size() < max_block_count_ ||
           buffer_list_.back().GetBackFree() > 0;
  });

  if (GetClosed()) {
    StreamClosedException::Check(true);
  }

  auto empty = buffer_list_.empty();

  Index written = 0;

  if (empty) {
    buffer_list_.push_back(Buffer(block_size_));
  }

  while (true) {
    if (buffer_list_.back().GetBackFree() == 0) {
      if (max_block_count_ > 0 && buffer_list_.size() == max_block_count_) {
        break;
      }
      buffer_list_.push_back(Buffer(block_size_));
    }
    auto& stream_buffer = buffer_list_.back();
    auto this_written =
        stream_buffer.PushBack(buffer + offset + written, size - written);
    written += this_written;
    if (written == size) {
      break;
    }
  }

  if (empty) {
    // By convention, there should be at most one consumer waiting. So
    // notify_one and notify_all should be the same.
    condition_variable_.notify_one();
  }

  return written;
}

void BufferStream::SetEof() {
  std::unique_lock lock(mutex_);

  eof_ = true;
  if (buffer_list_.empty()) {
    // By convention, there should be at most one consumer waiting. So
    // notify_one and notify_all should be the same.
    condition_variable_.notify_one();
  }
}

void BufferStream::DoClose() {
  CRU_STREAM_BEGIN_CLOSE
  SetClosed(true);
  condition_variable_.notify_all();
  buffer_list_.clear();
}
}  // namespace cru::io
