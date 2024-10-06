#include "cru/base/platform/win/StreamConvert.h"
#include "BrigdeComStream.h"
#include "Win32FileStreamPrivate.h"
#include "cru/base/Exception.h"
#include "cru/base/io/MemoryStream.h"
#include "cru/base/io/OpenFileFlag.h"
#include "cru/base/platform/win/ComAutoInit.h"
#include "cru/base/platform/win/Exception.h"
#include "cru/base/platform/win/Win32FileStream.h"

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
    return file_stream->GetPrivate_()->stream_;
  } else {
    return new BridgeComStream(stream);
  }
}
}  // namespace cru::platform::win
