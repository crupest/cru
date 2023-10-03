/**
 * Here are some notes about FileStream:
 *
 * 1. FileStream is currently implemented as a typedef of the corresponding
 * specific XxxFileStream class implemented on each platform and controlled with
 * preprocessor commands. There might be some other ways like proxy pattern but
 * I do this way for simplicity. So your duty to implement a new platform is to
 * define a new class and ensure it implements all the required interface. And
 * in this way you are free to expose any other additional interface like for
 * specific platform.
 *
 * 2. Since each platform defines their own way to open a file, especially the
 * flags to open a file, we have to define a common interface. I decide to
 * mimic Linux flags so on platforms where there is no direct support on certain
 * flags we try our best to simulate it and make a note for users.
 *
 * (TODO: Currently the problem is that when I implemented for Windows and UNIX
 * I didn't take this into consideration so I have to fix this inconsistency
 * later.)
 *
 * The requirement of FileStream:
 * 1. It must be derived from Stream, of course.
 * 2. It must have a constructor FileStream(String path, io::OpenFlag flags), so
 * user can construct one with the same interface.
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
#elif CRU_PLATFORM_EMSCRIPTEN
#include "../platform/web/WebFileStream.h"
namespace cru::io {
using FileStream = platform::web::WebFileStream;
}
#endif
