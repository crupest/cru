#include "cru/common/io/AutoReadStream.h"
#include "cru/common/io/Stream.h"

#include <thread>

namespace cru::io {

AutoReadStream::AutoReadStream(Stream* stream, bool auto_delete,
                               const AutoReadStreamOptions& options) {
  auto buffer_stream_options = options.GetBufferStreamOptions();
  stream_ = stream;
  size_per_read_ = buffer_stream_options.GetBlockSizeOrDefault();
  buffer_stream_ = std::make_unique<BufferStream>(buffer_stream_options);
  auto background_thread =
      std::thread(&AutoReadStream::BackgroundThreadRun, this);
  background_thread.detach();
}

AutoReadStream::~AutoReadStream() {
  if (auto_delete_) {
    delete stream_;
  }
}

bool AutoReadStream::CanSeek() { return false; }

Index AutoReadStream::Seek(Index offset, SeekOrigin origin) {
  throw StreamOperationNotSupportedException(
      u"AutoReadStream does not support seek.");
}

bool AutoReadStream::CanRead() { return true; }

Index AutoReadStream::Read(std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(buffer_stream_mutex_);
  return buffer_stream_->Read(buffer, offset, size);
}

bool AutoReadStream::CanWrite() { return stream_->CanWrite(); }

Index AutoReadStream::Write(const std::byte* buffer, Index offset, Index size) {
  return stream_->Write(buffer, offset, size);
}

void AutoReadStream::Flush() { stream_->Flush(); }

void AutoReadStream::Close() { stream_->Close(); }

void AutoReadStream::BackgroundThreadRun() {
  auto resolver = CreateResolver();
  std::vector<std::byte> buffer(size_per_read_);
  while (true) {
    try {
      auto read = stream_->Read(buffer.data(), buffer.size());
      auto self = resolver.Resolve();
      if (!self) {
        break;
      }
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
