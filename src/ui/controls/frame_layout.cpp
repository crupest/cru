#include "frame_layout.hpp"

namespace cru::ui::controls
{
    FrameLayout::FrameLayout() : Control(true)
    {

    }

    FrameLayout::~FrameLayout() = default;

    StringView FrameLayout::GetControlType() const
    {
        return control_type;
    }
}
