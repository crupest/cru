#pragma once

#include "../Buffer.h"
#include "../Exception.h"
#include "Stream.h"

#include <condition_variable>
#include <list>
#include <mutex>

namespace cru::io {
class WriteAfterEofException : public Exception {
 public:
  using Exception::Exception;
  ~WriteAfterEofException() override = default;
};

struct BufferStreamOptions {
  /**
   * Actually I have no ideas about the best value for this. May change it later
   * when I get some ideas.
   */
  constexpr static Index kDefaultBlockSize = 1024;

  /**
   * @brief The size of a single buffer allocated each time new space is needed.
   * Use default value if <= 0.
   *
   * When current buffer is full and there is no space for following data, a new
   * buffer will be allocated and appended to the buffer list. Note if sum size
   * of all buffers reaches the total_buffer_limit, no more buffer will be
   * allocated but wait.
   */
  Index block_size = 0;

  /**
   * @brief Total size limit of saved data in buffer. No limit if <= 0.
   *
   * The size will be floor(total_size_limit / block_size). When the buffer is
   * filled, it will block and wait for user to read to get free space of buffer
   * to continue read.
   */
  Index total_size_limit = 0;

  Index GetBlockSizeOrDefault() const {
    return block_size <= 0 ? kDefaultBlockSize : block_size;
  }

  Index GetMaxBlockCount() const { return total_size_limit / block_size; }
};

/**
 * @brief SPSC (Single Producer Single Consumer) buffer stream.
 *
 * If used by multiple producer or multiple consumer, the behavior is undefined.
 */
class BufferStream : public Stream {
 public:
  BufferStream(const BufferStreamOptions& options);

  ~BufferStream() override;

  bool CanSeek() override;
  Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;

  bool CanRead() override;
  Index Read(std::byte* buffer, Index offset, Index size) override;
  using Stream::Read;

  bool CanWrite() override;
  Index Write(const std::byte* buffer, Index offset, Index size) override;
  using Stream::Write;

  void SetEof();

 private:
  Index block_size_;
  Index max_block_count_;

  std::list<Buffer> buffer_list_;
  bool eof_;

  std::mutex mutex_;
  std::condition_variable condition_variable_;
};
}  // namespace cru::io
