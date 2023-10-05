#include "cru/common/io/OpenFileFlag.h"

namespace cru::io {
bool CheckOpenFileFlag(OpenFileFlag flags) {
  auto has = [flags](OpenFileFlag flag) { return flags & flag; };

  if ((has(OpenFileFlags::Append) || has(OpenFileFlags::Truncate) ||
       has(OpenFileFlags::Create)) &&
      !has(OpenFileFlags::Write)) {
    return false;
  }

  if (has(OpenFileFlags::Truncate) && has(OpenFileFlags::Append)) {
    return false;
  }

  return true;
}
}  // namespace cru::io
