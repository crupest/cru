#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "../../io/Base.h"
#include "../../io/Stream.h"
#include "Base.h"

#include <objidlbase.h>

namespace cru::platform::win {
class CRU_BASE_API Win32HandleStream : public io::Stream {
 public:
  Win32HandleStream(std::string_view path, io::OpenFileFlag flags);
  Win32HandleStream(HANDLE handle, bool auto_close, bool can_seek,
                    bool can_read, bool can_write);
  Win32HandleStream(Win32Handle&& handle, bool can_seek, bool can_read,
                    bool can_write);
  ~Win32HandleStream() override;

 protected:
  Index DoSeek(Index offset, SeekOrigin origin) override;
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;

 public:
  const Win32Handle& GetHandle() { return handle_; }

  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

 private:
  void DoClose();

 private:
  Win32Handle handle_;
};

CRU_BASE_API IStream* ToComStream(io::Stream* stream);

class CRU_BASE_API ComStream : public io::Stream {
 public:
  ComStream(std::string_view path, io::OpenFileFlag flags);
  ComStream(IStream* com_stream, bool auto_release, bool can_seek,
            bool can_read, bool can_write);
  ~ComStream() override;

 protected:
  Index DoSeek(Index offset, SeekOrigin origin) override;
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;

 public:
  IStream* GetComStream() { return stream_; }

  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

 private:
  void DoClose();

 private:
  IStream* stream_;
  bool auto_release_;
};
}  // namespace cru::platform::win
