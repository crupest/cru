#pragma once

#if !defined(__unix) && !defined(__APPLE__)
#error "This file can only be included on unix."
#endif

#include "../../Bitmask.h"

#include <sys/types.h>
#include <functional>

namespace cru::platform::unix {
class UnixFileDescriptor {
 public:
  UnixFileDescriptor();
  /**
   * \param close Default to POSIX close function.
   */
  explicit UnixFileDescriptor(int descriptor, bool auto_close = true,
                              std::function<int(int)> close = {});
  ~UnixFileDescriptor();

  UnixFileDescriptor(const UnixFileDescriptor& other) = delete;
  UnixFileDescriptor& operator=(const UnixFileDescriptor& other) = delete;

  UnixFileDescriptor(UnixFileDescriptor&& other) noexcept;
  UnixFileDescriptor& operator=(UnixFileDescriptor&& other) noexcept;

  bool IsValid() const;
  void EnsureValid() const;
  int GetValue() const;
  void Close();

  bool IsAutoClose() const { return auto_close_; }
  void SetAutoClose(bool auto_close) { this->auto_close_ = auto_close; }

  explicit operator bool() const { return this->IsValid(); }
  operator int() const { return this->GetValue(); }

  /**
   * If O_NONBLOCK is set and EAGAIN or EWOULDBLOCK is returned, -1 is returned.
   */
  ssize_t Read(void* buffer, size_t size);
  /**
   * If O_NONBLOCK is set and EAGAIN or EWOULDBLOCK is returned, -1 is returned.
   */
  ssize_t Write(const void* buffer, size_t size);
  void SetFileDescriptorFlags(int flags);

 private:
  bool DoClose();

 private:
  int descriptor_;
  bool auto_close_;
  std::function<int(int)> close_;
};

struct UniDirectionalUnixPipeResult {
  UnixFileDescriptor read;
  UnixFileDescriptor write;
};

CRU_DEFINE_BITMASK(UnixPipeFlag) {
  constexpr static auto NonBlock = UnixPipeFlag::FromOffset(1);
};

UniDirectionalUnixPipeResult OpenUniDirectionalPipe(UnixPipeFlag flags = {});
}  // namespace cru::platform::unix
