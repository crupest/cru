#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_EMSCRIPTEN

#include "../../io/Stream.h"

namespace cru::platform::web {
/**
 *  \remark Web just hates filesystems. But luckily emscripten just creates a
 * good simulation of it. All we need to do is to use the C api and let
 * emscripten take care of it for us.
 */
class WebFileStream : public io::Stream {
 public:
 // TODO: go on this!
};
}  // namespace cru::platform::web

#endif
