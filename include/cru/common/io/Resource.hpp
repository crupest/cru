#pragma once

#include "../Base.hpp"
#include "Stream.hpp"

#include <memory>

namespace cru::io {
std::unique_ptr<Stream> CreateStreamFromResourcePath(const String& path);
}
