#include "cru_debug.h"

#include "system_headers.h"

namespace cru::debug
{
    void DebugMessage(const StringView& message)
    {
        ::OutputDebugStringW(message.data());
    }
}
