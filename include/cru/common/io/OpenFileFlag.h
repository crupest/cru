#pragma once

#include "../Bitmask.h"

namespace cru::io {
namespace details {
struct OpenFileFlagTag {};
}  // namespace details
using OpenFileFlag = Bitmask<details::OpenFileFlagTag>;

struct OpenFileFlags {
  static constexpr OpenFileFlag Read{0x1};
  static constexpr OpenFileFlag Write{0x2};
  static constexpr OpenFileFlag Append{0x4};
  static constexpr OpenFileFlag Create{0x8};
  static constexpr OpenFileFlag ThrowOnExist{0x10};
};
}  // namespace cru::io
