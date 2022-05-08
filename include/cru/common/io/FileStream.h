#pragma once

#include "../PreConfig.h"

#ifdef CRU_PLATFORM_UNIX
#include "../platform/unix/UnixFileStream.h"
namespace cru::io {
using FileStream = platform::unix::UnixFileStream;
}
#elif CRU_PLATFORM_WINDOWS
#include "../platform/win/Win32FileStream.h"
namespace cru::io {
using FileStream = platform::win::Win32FileStream;
}
#endif
