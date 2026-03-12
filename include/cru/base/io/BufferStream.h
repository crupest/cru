#pragma once

#include "Stream.h"

#include <condition_variable>
#include <list>
#include <mutex>

namespace cru::io {
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

  Index GetMaxBlockCount() const {
    return total_size_limit / GetBlockSizeOrDefault();
  }
};

/**
 * @brief MPMC (Multiple Producer Multiple Consumer) buffer stream.
 */
class CRU_BASE_API BufferStream : public Stream {
 public:
  explicit BufferStream(const BufferStreamOptions& options = {});
  ~BufferStream() override;

  void WriteEof();

 protected:
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;
  void DoClose() override;

 private:
  struct Block {
    std::byte* buffer;
    Index size;
    Index start;
    Index end;

    explicit Block(Index size);

    CRU_DELETE_COPY(Block)

    Block(Block&& other) noexcept;
    Block& operator=(Block&& other) noexcept;

    ~Block();

    Index Read(std::byte* des, Index si);
    Index Write(const std::byte* src, Index si);
    bool IsFull() const;
    bool IsEmpty() const;
  };

  Index block_size_;
  Index max_block_count_;

  std::list<Block> buffer_list_;
  bool eof_written_;

  std::mutex mutex_;
  std::condition_variable read_cv_;
  std::condition_variable write_cv_;
};
}  // namespace cru::io
