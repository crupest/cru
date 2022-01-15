#include "cru/common/ErrnoException.hpp"

#include <errno.h>

namespace cru {
ErrnoException::ErrnoException(const String& message)
    : ErrnoException(message, errno) {}

ErrnoException::ErrnoException(const String& message, int errno_code)
    : Exception(message), errno_code_(errno_code) {}
}  // namespace cru
