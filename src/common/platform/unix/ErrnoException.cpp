#include "cru/common/platform/unix/ErrnoException.h"

#include "cru/common/Format.h"

#include <errno.h>

namespace cru::platform::unix {
ErrnoException::ErrnoException(const String& message)
    : ErrnoException(message, errno) {}

ErrnoException::ErrnoException(const String& message, int errno_code)
    : PlatformException(Format(u"{}. Errno is {}.", message, errno_code)),
      errno_code_(errno_code) {}
}  // namespace cru::platform::unix
