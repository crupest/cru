#pragma once

#ifdef CRU_PLATFORM_UNIX
#include "UnixFileStream.hpp"
namespace cru::io {
using FileStream = UnixFileStream;
}
#elif CRU_PLATFORM_WINDOWS
#include "../platform/win/Win32FileStream.hpp"
namespace cru::io {
using FileStream = platform::win::Win32FileStream;
}
#endif
