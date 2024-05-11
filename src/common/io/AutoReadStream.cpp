#include "cru/common/io/AutoReadStream.h"
#include <vector>
#include "cru/common/io/Stream.h"

namespace cru::io {

AutoReadStream::AutoReadStream(Stream* stream, bool auto_delete,
                               const AutoReadStreamOptions& options) {
  background_thread_ = std::thread(&AutoReadStream::BackgroundThreadRun, this);
}

bool AutoReadStream::CanSeek() { return false; }

Index AutoReadStream::Seek(Index offset, SeekOrigin origin) {
  throw StreamOperationNotSupportedException(
      u"AutoReadStream does not support seek.");
}

bool AutoReadStream::CanWrite() { return stream_->CanWrite(); }

Index AutoReadStream::Write(const std::byte* buffer, Index offset, Index size) {
  return stream_->Write(buffer, offset, size);
}

void AutoReadStream::Flush() { stream_->Flush(); }

void AutoReadStream::BackgroundThreadRun() {
  std::unique_lock<std::mutex> lock(buffer_mutex_);
  std::vector<std::byte>* buffer = nullptr;
  if (!buffer_list_.empty()) {
  }
  stream_->Read();
}

}  // namespace cru::io
