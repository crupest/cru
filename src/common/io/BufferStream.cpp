#include "cru/common/io/BufferStream.h"
#include "cru/common/io/Stream.h"

namespace cru::io {
BufferStream::BufferStream(const BufferStreamOptions& options) {
  block_size_ =
      options.block_size <= 0 ? kDefaultBlockSize : options.block_size;
  total_size_limit_ = options.total_size_limit < 0 ? kDefaultTotalSizeLimit
                                                   : options.total_size_limit;
  block_count_limit_ = total_size_limit_ / block_size_;

  eof_ = false;
}

bool BufferStream::CanSeek() {
  CheckClosed();
  return false;
}

Index BufferStream::Seek(Index offset, SeekOrigin origin) {
  CheckClosed();
  throw StreamOperationNotSupportedException(
      u"BufferStream does not support seeking.");
}

bool BufferStream::CanRead() {
  CheckClosed();
  return true;
}

Index BufferStream::Read(std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(mutex_);

  condition_variable_.wait(lock,
                           [this] { return !buffer_list_.empty() || eof_; });

  if (eof_) {
    return 0;
  }

  Index written_size = 0;
  auto current_offset = offset;
  auto rest_size = size;

  while (!buffer_list_.empty()) {
    auto& stream_buffer = buffer_list_.front();
    auto this_written_size =
        stream_buffer.PopFront(buffer + current_offset, rest_size);
    if (stream_buffer.GetUsedSize() == 0) {
      buffer_list_.pop_front();
    }
    written_size += this_written_size;
    rest_size -= this_written_size;
    current_offset += this_written_size;
    if (rest_size == 0) {
      break;
    }
  }

  return written_size;
}

}  // namespace cru::io
