#pragma once

#ifdef CRU_PLATFORM_UNIX
#include "../platform/unix/UnixFileStream.hpp"
namespace cru::io {
using FileStream = platform::unix::UnixFileStream;
}
#elif CRU_PLATFORM_WINDOWS
#include "../platform/win/Win32FileStream.hpp"
namespace cru::io {
using FileStream = platform::win::Win32FileStream;
}
#endif
