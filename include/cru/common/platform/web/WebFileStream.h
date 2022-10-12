#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_EMSCRIPTEN

#include "../../io/Stream.h"
#include "cru/common/io/OpenFileFlag.h"

namespace cru::platform::web {
/**
 *  \remark Web just hates filesystems. But luckily emscripten just creates a
 * good simulation of it. All we need to do is to use the C api and let
 * emscripten take care of it for us.
 */
class WebFileStream : public io::Stream {
 public:
  WebFileStream(String path, io::OpenFileFlag flags);

  ~WebFileStream() override;

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
  String path_;
  io::OpenFileFlag flags_;

  FILE* file_;
};
}  // namespace cru::platform::web

#endif
