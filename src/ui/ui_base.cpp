#include "ui_base.hpp"

#include "system_headers.hpp"

namespace cru::ui
{
    bool IsKeyDown(const int virtual_code)
    {
        const auto result = ::GetKeyState(virtual_code);
        return (static_cast<unsigned short>(result) & 0x8000) != 0;
    }

    bool IsKeyToggled(const int virtual_code)
    {
        const auto result = ::GetKeyState(virtual_code);
        return (static_cast<unsigned short>(result) & 1) != 0;
    }
}
