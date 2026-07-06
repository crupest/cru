#pragma once

#include "../Base.h"
#include "../Bitmask.h"

namespace cru::io {
class CRU_BASE_API FileNotExistException : public Exception {
 public:
  using Exception::Exception;
};

CRU_DEFINE_BITMASK(OpenFileFlag) {
  /**
   * \brief for reading
   * If the file does not exist, FileNotExistException should be thrown.
   */
  static constexpr OpenFileFlag Read = OpenFileFlag::FromOffset(1);

  /**
   * \brief for writing
   * If the file does not exist and Create is not specified,
   * FileNotExistException should be thrown.
   */
  static constexpr OpenFileFlag Write = OpenFileFlag::FromOffset(2);

  /**
   * \brief when writing, seek to end first
   * Only effective for writing.
   */
  static constexpr OpenFileFlag Append = OpenFileFlag::FromOffset(3);

  /**
   * \brief when writing, truncate the file to empty
   * Only effective for writing.
   * So the content is erased! Be careful!
   */
  static constexpr OpenFileFlag Truncate = OpenFileFlag::FromOffset(4);

  /**
   * \brief when writing, if the file does not exist, create one
   * Only effective for writing. When file does not exist, FileNotExistException
   * will NOT be thrown and a new file will be created.
   */
  static constexpr OpenFileFlag Create = OpenFileFlag::FromOffset(5);
};

/**
 * Append, Truncate, Create must be used with Write.
 * Append and Truncate must not be used together.
 */
bool CheckOpenFileFlag(OpenFileFlag flags);
}  // namespace cru::io
