#pragma once

#include "../Buffer.h"
#include "Stream.h"

#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>

namespace cru::io {
struct AutoReadStreamOptions {
  /**
   * @brief The size of a single buffer allocated each time new space is needed.
   * Use default value if <= 0.
   *
   * When current buffer is full and there is no space for following data, a new
   * buffer will be allocated and appended to the buffer list. Note if sum size
   * of all buffers reaches the total_buffer_size, no more buffer will be
   * allocated but wait.
   */
  int buffer_block_size = 0;

  /**
   * @brief Total size limit of saved data in buffer. Use default value if < 0.
   * No limit if == 0.
   *
   * When the buffer is filled, it will block and wait for user to read to get
   * free space of buffer to continue read.
   */
  int total_buffer_size = 0;
};

/**
 * @brief A stream that wraps another stream and auto read it into a buffer in a
 * background thread.
 */
class CRU_BASE_API AutoReadStream : public Stream {
 private:
  class BufferBlock {};

 public:
  /**
   * @brief Wrap a stream and auto read it in background.
   * @param stream The stream to auto read.
   * @param auto_delete Whether to delete the stream object in destructor.
   * @param options Options to modify the behavior.
   */
  AutoReadStream(
      Stream* stream, bool auto_delete,
      const AutoReadStreamOptions& options = AutoReadStreamOptions());

  ~AutoReadStream() override;

 public:
  bool CanSeek() override;
  Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;

  bool CanRead() = 0;
  virtual Index Read(std::byte* buffer, Index offset, Index size) = 0;

  bool CanWrite() override;
  Index Write(const std::byte* buffer, Index offset, Index size) override;

  void Flush() override;

  void Close() = 0;

 private:
  void BackgroundThreadRun();

 private:
  Stream* stream_;
  bool auto_delete_;

  int buffer_block_size_;
  int total_buffer_size_;

  std::mutex buffer_mutex_;
  std::condition_variable buffer_condition_variable_;
  std::list<Buffer> buffer_list_;

  std::thread background_thread_;
};
}  // namespace cru::io
