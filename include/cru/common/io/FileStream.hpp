#pragma once

#ifdef CRU_PLATFORM_UNIX
#include "UnixFileStream.hpp"
namespace cru::io {
using FileStream = UnixFileStream;
}
#endif
