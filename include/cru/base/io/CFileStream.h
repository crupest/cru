#pragma once

#include "Stream.h"

#include <cstdio>

namespace cru::io {
class CRU_BASE_API CFileStream : public Stream {
 public:
  CFileStream(const char* path, const char* mode);
  explicit CFileStream(std::FILE* file, bool readable = true,
                       bool writable = true, bool auto_close = true);

  CRU_DELETE_COPY(CFileStream)
  CRU_DELETE_MOVE(CFileStream)

  ~CFileStream() override;

 public:
  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

  std::FILE* GetHandle() const;

 protected:
  Index DoSeek(Index offset, SeekOrigin origin) override;
  Index DoTell() override;
  void DoRewind() override;
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;
  void DoFlush() override;

 private:
  void DoClose();

 private:
  std::FILE* file_;
  bool auto_close_;
};
}  // namespace cru::io
