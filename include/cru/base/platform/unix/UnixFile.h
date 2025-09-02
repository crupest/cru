#pragma once

#if !defined(__unix) && !defined(__APPLE__)
#error "This file can only be included on unix."
#endif

#include "../../Bitmask.h"

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
  int GetValue() const;
  void Close();

  bool IsAutoClose() const { return auto_close_; }
  void SetAutoClose(bool auto_close) { this->auto_close_ = auto_close; }

  explicit operator bool() const { return this->IsValid(); }
  operator int() const { return this->GetValue(); }

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

namespace details {
struct UnixPipeFlagTag;
}
using UnixPipeFlag = Bitmask<details::UnixPipeFlagTag>;
struct UnixPipeFlags {
  constexpr static auto NonBlock = UnixPipeFlag::FromOffset(1);
};

UniDirectionalUnixPipeResult OpenUniDirectionalPipe(UnixPipeFlag flags = {});
}  // namespace cru::platform::unix
