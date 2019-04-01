#pragma once
#include "cru/common/pre_config.hpp"

#include <string>
#include <string_view>

namespace cru::platform::util {
std::string ToUtf8String(const std::wstring_view& string);
}
