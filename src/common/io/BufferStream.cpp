#include "cru/common/io/BufferStream.h"
#include <memory>
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
  return true; }

Index BufferStream::Read(std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(mutex_);

  Index written_size = 0;

  if (eof_ && buffer_list_.empty()) {
    return 0;
  }

  while (!buffer_list_.empty()) {
    
  }
}

}  // namespace cru::io
