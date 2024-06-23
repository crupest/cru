#pragma once

#include "BufferStream.h"
#include "Stream.h"

#include <mutex>
#include <thread>

namespace cru::io {
struct AutoReadStreamOptions {
  /**
   * @brief Will be passed to BufferStreamOptions::block_size.
   */
  Index block_size = 0;

  /**
   * @brief Will be passed to BufferStreamOptions::total_size_limit.
   */
  Index total_size_limit = 0;

  BufferStreamOptions GetBufferStreamOptions() const {
    BufferStreamOptions options;
    options.block_size = block_size;
    options.total_size_limit = total_size_limit;
    return options;
  }
};

/**
 * @brief A stream that wraps another stream and auto read it into a buffer in a
 * background thread.
 */
class CRU_BASE_API AutoReadStream : public Stream {
 public:
  /**
   * @brief Wrap a stream and auto read it in background.
   * @param stream The stream to auto read.
   * @param auto_delete Whether to delete the stream object in destructor.
   * @param options Options to modify the behavior.
   */
  AutoReadStream(
      Stream* stream, bool auto_close, bool auto_delete,
      const AutoReadStreamOptions& options = AutoReadStreamOptions());

  ~AutoReadStream() override;

 public:
  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

  void BeginToDrop(bool auto_close = true, bool auto_delete = true);

 protected:
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;
  void DoFlush() override;

 private:
  void DoClose();

  void BackgroundThreadRun();

 private:
  Stream* stream_;
  bool auto_close_;
  bool auto_delete_;

  Index size_per_read_;
  std::unique_ptr<BufferStream> buffer_stream_;

  std::thread background_thread_;
};
}  // namespace cru::io
