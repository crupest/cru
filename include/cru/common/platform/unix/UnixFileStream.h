#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_UNIX

#include "../../io/Stream.h"

namespace cru::platform::unix {
class UnixFileStream : public io::Stream {
 private:
  static constexpr auto kLogTag = u"cru::platform::unix::UnixFileStream";

 public:
  UnixFileStream(const char* path, int oflag);
  UnixFileStream(int fd, bool can_seek, bool can_read, bool can_write,
                 bool auto_close);

  CRU_DELETE_COPY(UnixFileStream)
  CRU_DELETE_MOVE(UnixFileStream)

  ~UnixFileStream() override;

 public:
  bool CanSeek() override;
  Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;

  bool CanRead() override;
  Index Read(std::byte* buffer, Index offset, Index size) override;
  using Stream::Read;

  bool CanWrite() override;
  Index Write(const std::byte* buffer, Index offset, Index size) override;
  using Stream::Write;

  void Close() override;

  int GetFileDescriptor() const { return file_descriptor_; }

 private:
  void CheckClosed();

 private:
  int file_descriptor_;  // -1 for no file descriptor
  bool can_seek_;
  bool can_read_;
  bool can_write_;
  bool auto_close_;
};
}  // namespace cru::platform::unix

#endif
