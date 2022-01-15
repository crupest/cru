#pragma once

#include "../PreConfig.hpp"

#ifdef CRU_PLATFORM_UNIX

#include "../String.hpp"
#include "OpenFileFlag.hpp"
#include "Stream.hpp"

namespace cru::io {
class UnixFileStream : public Stream {
 public:
  UnixFileStream(String path, OpenFileFlag flags);

  CRU_DELETE_COPY(UnixFileStream)
  CRU_DELETE_MOVE(UnixFileStream)

  ~UnixFileStream() override;

 public:
  bool CanSeek() override;
  Index Tell() override;
  void Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;

  bool CanRead() override;
  Index Read(std::byte* buffer, Index offset, Index size) override;

  bool CanWrite() override;
  Index Write(const std::byte* buffer, Index offset, Index size) override;

  void Close() override;

 private:
  void CheckClosed();

 private:
  String path_;
  OpenFileFlag flags;
  int file_descriptor_;
  bool closed_ = false;
};
}  // namespace cru::io

#endif
