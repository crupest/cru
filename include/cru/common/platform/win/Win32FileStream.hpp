#pragma once

#include "../../PreConfig.hpp"

#ifdef CRU_PLATFORM_WINDOWS

#include "../../String.hpp"
#include "../../io/OpenFileFlag.hpp"
#include "../../io/Stream.hpp"

namespace cru::platform::win {
namespace details {
class Win32FileStreamPrivate;
}

class CRU_BASE_API Win32FileStream : public io::Stream {
 public:
  Win32FileStream(String path, io::OpenFileFlag flags);

  CRU_DELETE_COPY(Win32FileStream)
  CRU_DELETE_MOVE(Win32FileStream)

  ~Win32FileStream() override;

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
  bool closed_ = false;

  details::Win32FileStreamPrivate* p_;
};
}  // namespace cru::platform::win

#endif
