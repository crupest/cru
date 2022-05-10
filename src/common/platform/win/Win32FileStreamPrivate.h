#include "cru/common/platform/win/WinPreConfig.h"

#include <objidl.h>

namespace cru::platform::win {

namespace details {
struct Win32FileStreamPrivate {
  IStream* stream_ = nullptr;
};
}  // namespace details

}  // namespace cru::platform::win
