#include "cru/common/platform/win/StreamConvert.h"
#include "BrigdeComStream.h"
#include "cru/common/Exception.h"
#include "cru/common/io/MemoryStream.h"
#include "cru/common/io/OpenFileFlag.h"
#include "cru/common/platform/win/ComAutoInit.h"
#include "cru/common/platform/win/Exception.h"
#include "cru/common/platform/win/Win32FileStream.h"

#include <shlwapi.h>
#include <winnt.h>

namespace cru::platform::win {
IStream* ConvertStreamToComStream(io::Stream* stream) {
  static ComAutoInit com_auto_init;

  if (auto memory_stream = dynamic_cast<io::MemoryStream*>(stream)) {
    return SHCreateMemStream(
        reinterpret_cast<const BYTE*>(memory_stream->GetBuffer()),
        memory_stream->GetSize());
  } else if (auto file_stream = dynamic_cast<Win32FileStream*>(stream)) {
    auto path = file_stream->GetPath();
    auto flags = file_stream->GetOpenFileFlags();
    DWORD grfMode = STGM_SHARE_DENY_NONE | STGM_FAILIFTHERE;
    if (flags & io::OpenFileFlags::Read) {
      if (flags & io::OpenFileFlags::Write) {
        grfMode |= STGM_READWRITE;
      } else {
        grfMode |= STGM_READ;
      }
    } else {
      if (flags & io::OpenFileFlags::Write) {
        grfMode |= STGM_WRITE;
      } else {
        throw Exception(u"Stream must be readable or writable.");
      }
    }

    IStream* result;

    ThrowIfFailed(SHCreateStreamOnFileEx(
        path.WinCStr(), grfMode, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &result));

    LARGE_INTEGER position;
    position.QuadPart = stream->Tell();
    result->Seek(position, STREAM_SEEK_SET, NULL);

    return result;
  } else {
    return new BridgeComStream(stream);
  }
}
}  // namespace cru::platform::win
