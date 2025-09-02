#pragma once

#if !defined(__unix) && !defined(__APPLE__)
#error "This file can only be included on unix."
#endif

#include "../../io/Stream.h"
#include "UnixFile.h"

namespace cru::platform::unix {
class UnixFileStream : public io::Stream {
 private:
  static constexpr auto kLogTag = u"cru::platform::unix::UnixFileStream";

 public:
  UnixFileStream(const char* path, int oflag, mode_t mode = 0660);
  UnixFileStream(UnixFileDescriptor fd, bool can_seek, bool can_read,
                 bool can_write);
  ~UnixFileStream() override;

 public:
  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

  int GetFileDescriptor() const { return file_descriptor_; }

 protected:
  Index DoSeek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;

 private:
  void DoClose();

 private:
  UnixFileDescriptor file_descriptor_;
};
}  // namespace cru::platform::unix
