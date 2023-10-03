#pragma once

#include "../Bitmask.h"

namespace cru::io {
namespace details {
struct OpenFileFlagTag {};
}  // namespace details
using OpenFileFlag = Bitmask<details::OpenFileFlagTag>;

struct OpenFileFlags {
  /**
   * \brief for reading
   */
  static constexpr OpenFileFlag Read{0x1};

  /**
   * \brief for writing
   */
  static constexpr OpenFileFlag Write{0x2};

  /**
   * \brief when writing, seek to end first
   * Only effective for writing.
   */
  static constexpr OpenFileFlag Append{0x4};

  /**
   * \brief when writing, truncate the file to empty
   * Only effective for writing.
   * So the content is erased! Be careful!
   */
  static constexpr OpenFileFlag Truncate{0x8};

  /**
   * \brief when writing, if the file does not exist, create one
   * Only effective for writing.
   */
  static constexpr OpenFileFlag Create{0x10};

  /**
   *  TODO: ???
   */
  static constexpr OpenFileFlag Exclusive{0x20};
};
}  // namespace cru::io
