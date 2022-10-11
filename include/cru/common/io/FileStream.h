/**
 * Here are some notes about FileStream:
 *
 * 1. FileStream is currently implemented as a typedef of the corresponding
 * specific XxxFileStream class implemented on each platform and controled with
 * preprocessor commands. There might be some other ways like proxy pattern but
 * I do this way for simplicity. So your duty to implement a new platform is to
 * define a new class and ensure it implements all the required interface. And
 * in this way you are free to expose any other additional interface like for
 * specific platform.
 *
 * 2. Since each platform defines their own way to open a file, especially the
 * flags to open a file, we have to define a common interface. I decide to
 * mimic the C++ std stream open flags
 * (https://en.cppreference.com/w/cpp/io/basic_filebuf/open) so on platforms
 * where there is no direct support on certain flags we try our best to
 * simulate it and make a note for users. The difference between cru design and
 * std design is:
 *   1. cru deletes `binary` flag because Stream does not have a so-called
 * _text_ mode and it is always byte based.
 *   2. cru adds `OpenFileFlags::Create` and `OpenFileFlags::Exclusive` flags
 * mimicking Linux behavior which does not exist in std.
 *
 * (TODO: Currently the problem is that when I implemented for Windows and UNIX
 * I didn't take this into consideration so I have to fix this inconsistency
 * later.)
 */

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
