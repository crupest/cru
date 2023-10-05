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
  bool CanSeek() override;
  Index Seek(Index offset, SeekOrigin origin = SeekOrigin::Current) override;
  Index Tell() override;
  void Rewind() override;

  bool CanRead() override;
  Index Read(std::byte* buffer, Index offset, Index size) override;

  bool CanWrite() override;
  Index Write(const std::byte* buffer, Index offset, Index size) override;

  void Flush() override;

  void Close() override;

  std::FILE* GetHandle() const;

 private:
  void CheckClosed();

 private:
  std::FILE* file_;
  bool readable_;
  bool writable_;
  bool auto_close_;
};
}  // namespace cru::io
