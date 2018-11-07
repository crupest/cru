#include "cru_debug.hpp"

#include "system_headers.hpp"

namespace cru::debug
{
    void DebugMessage(const StringView& message)
    {
        ::OutputDebugStringW(message.data());
    }
}
