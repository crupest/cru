#include "cru/base/io/BufferStream.h"
#include "cru/base/io/Stream.h"

namespace cru::io {
BufferStream::BufferStream(const BufferStreamOptions& options)
    : Stream(false, true, true) {
  block_size_ = options.GetBlockSizeOrDefault();
  max_block_count_ = options.GetMaxBlockCount();
  eof_written_ = false;
}

BufferStream::~BufferStream() {}

Index BufferStream::DoRead(std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(mutex_);

  read_cv_.wait(lock, [this] {
    return eof_written_ || !buffer_list_.empty() || IsClosed();
  });

  CheckClosed();

  if (buffer_list_.empty() && eof_written_) {
    return kEOF;
  }

  auto full_previously = max_block_count_ > 0 &&
                         buffer_list_.size() == max_block_count_ &&
                         buffer_list_.back().IsFull();

  Index read = 0;

  while (!buffer_list_.empty()) {
    auto& stream_buffer = buffer_list_.front();
    auto this_read = stream_buffer.Read(buffer + offset + read, size - read);
    if (stream_buffer.IsEmpty()) {
      buffer_list_.pop_front();
    }
    read += this_read;
    if (read == size) {
      break;
    }
  }

  if (full_previously && buffer_list_.size() < max_block_count_) {
    write_cv_.notify_all();
  }

  return read;
}

Index BufferStream::DoWrite(const std::byte* buffer, Index offset, Index size) {
  std::unique_lock lock(mutex_);

  write_cv_.wait(lock, [this] {
    return eof_written_ || max_block_count_ <= 0 ||
           buffer_list_.size() < max_block_count_ ||
           !buffer_list_.back().IsFull() || IsClosed();
  });

  CheckClosed();

  if (eof_written_) {
    throw StreamIOException(
        this, "Stream has been set eof. Can't write to it any more.");
  }

  auto empty_previously = buffer_list_.empty();

  Index written = 0;

  if (empty_previously) {
    buffer_list_.push_back(Block(block_size_));
  }

  while (true) {
    if (buffer_list_.back().IsFull()) {
      if (max_block_count_ > 0 && buffer_list_.size() == max_block_count_) {
        break;
      }
      buffer_list_.push_back(Block(block_size_));
    }
    auto& stream_buffer = buffer_list_.back();
    auto this_written =
        stream_buffer.Write(buffer + offset + written, size - written);
    written += this_written;
    if (written == size) {
      break;
    }
  }

  if (empty_previously) {
    read_cv_.notify_all();
  }

  return written;
}

void BufferStream::DoClose() {
  std::unique_lock lock(mutex_);

  buffer_list_.clear();
  read_cv_.notify_all();
  write_cv_.notify_all();
}

void BufferStream::WriteEof() {
  std::unique_lock lock(mutex_);

  eof_written_ = true;
  read_cv_.notify_all();
  write_cv_.notify_all();
}

BufferStream::Block::Block(Index size)
    : buffer(new std::byte[size]), size(size), start(0), end(0) {}

BufferStream::Block::Block(Block&& other) noexcept
    : buffer(other.buffer),
      size(other.size),
      start(other.start),
      end(other.end) {
  other.buffer = nullptr;
  other.size = other.start = other.end = 0;
}

BufferStream::Block& BufferStream::Block::operator=(Block&& other) noexcept {
  if (this != &other) {
    delete[] buffer;
    buffer = other.buffer;
    size = other.size;
    start = other.start;
    end = other.end;
    other.buffer = nullptr;
    other.size = other.start = other.end = 0;
  }
  return *this;
}

BufferStream::Block::~Block() { delete[] buffer; }

Index BufferStream::Block::Read(std::byte* des, Index si) {
  si = std::min(si, end - start);
  std::memcpy(des, buffer + start, si);
  start += si;
  return si;
}

Index BufferStream::Block::Write(const std::byte* src, Index si) {
  si = std::min(si, size - end);
  std::memcpy(buffer + end, src, si);
  end += si;
  return si;
}

bool BufferStream::Block::IsFull() const { return end == size; }

bool BufferStream::Block::IsEmpty() const { return end == start; }

}  // namespace cru::io
