#include "cru/base/io/AutoReadStream.h"
#include "cru/base/io/Stream.h"

#include <thread>

namespace cru::io {

AutoReadStream::AutoReadStream(Stream* stream, bool auto_delete,
                               const AutoReadStreamOptions& options)
    : Stream(false, true, stream->CanSeek()) {
  auto buffer_stream_options = options.GetBufferStreamOptions();
  stream_ = stream;
  size_per_read_ = buffer_stream_options.GetBlockSizeOrDefault();
  buffer_stream_ = std::make_unique<BufferStream>(buffer_stream_options);
  background_thread_ = std::thread(&AutoReadStream::BackgroundThreadRun, this);
}

AutoReadStream::~AutoReadStream() {
  if (auto_delete_) {
    delete stream_;
  }
}

Index AutoReadStream::DoRead(std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(buffer_stream_mutex_);
  return buffer_stream_->Read(buffer, offset, size);
}

Index AutoReadStream::DoWrite(const std::byte* buffer, Index offset,
                              Index size) {
  return stream_->Write(buffer, offset, size);
}

void AutoReadStream::DoFlush() { stream_->Flush(); }

void AutoReadStream::DoClose() {}

void AutoReadStream::BackgroundThreadRun() {
  std::vector<std::byte> buffer(size_per_read_);
  while (true) {
    try {
      auto read = stream_->Read(buffer.data(), buffer.size());
      if (read == 0) {
        buffer_stream_->SetEof();
        break;
      } else {
        buffer_stream_->Write(buffer.data(), read);
      }
    } catch (const StreamAlreadyClosedException& exception) {
      buffer_stream_->SetEof();
      break;
    }
  }
}

}  // namespace cru::io
