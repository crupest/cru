#include "cru/common/platform/web/WebFileStream.h"
#include "cru/common/io/OpenFileFlag.h"

#include <cstdio>

namespace cru::platform::web {
WebFileStream::WebFileStream(String path, io::OpenFileFlag flags)
    : path_(std::move(path)), flags_(flags) {
}

}  // namespace cru::platform::web
