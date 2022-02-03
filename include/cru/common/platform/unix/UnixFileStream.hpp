#pragma once

#include "../../PreConfig.hpp"

#ifdef CRU_PLATFORM_UNIX

#include "../../String.hpp"
#include "../../io/OpenFileFlag.hpp"
#include "../../io/Stream.hpp"

namespace cru::platform::unix {
class UnixFileStream : public io::Stream {
 public:
  UnixFileStream(String path, io::OpenFileFlag flags);

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

 private:
  void CheckClosed();

 private:
  String path_;
  io::OpenFileFlag flags_;
  int file_descriptor_;
  bool closed_ = false;
};
}  // namespace cru::platform::unix

#endif
