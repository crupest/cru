#pragma once

#ifdef CRU_PLATFORM_WINDOWS

#include "../../io/OpenFileFlag.h"
#include "../../io/Stream.h"

namespace cru::platform::win {
namespace details {
class Win32FileStreamPrivate;
}

class CRU_BASE_API Win32FileStream : public io::Stream {
 public:
  Win32FileStream(std::string path, io::OpenFileFlag flags);
  ~Win32FileStream() override;

 protected:
  Index DoSeek(Index offset, SeekOrigin origin) override;
  Index DoRead(std::byte* buffer, Index offset, Index size) override;
  Index DoWrite(const std::byte* buffer, Index offset, Index size) override;

 public:
  std::string GetPath() const { return path_; }
  io::OpenFileFlag GetOpenFileFlags() const { return flags_; }

  details::Win32FileStreamPrivate* GetPrivate_() { return p_; }

  CRU_STREAM_IMPLEMENT_CLOSE_BY_DO_CLOSE

 private:
  void DoClose();

 private:
  std::string path_;
  io::OpenFileFlag flags_;

  details::Win32FileStreamPrivate* p_;
};
}  // namespace cru::platform::win

#endif
