#include "input_util.hpp"

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

    bool IsAnyMouseButtonDown()
    {
        return IsKeyDown(VK_LBUTTON) || IsKeyDown(VK_RBUTTON) || IsKeyDown(VK_MBUTTON);
    }
}
