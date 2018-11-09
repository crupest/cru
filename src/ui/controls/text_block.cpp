#include "text_block.hpp"

#include "ui/window.hpp"
#include "ui/predefine.hpp"

namespace cru::ui::controls
{
    TextBlock::TextBlock() : TextControl(
        predefine::GetPredefineResourceComPtr<IDWriteTextFormat>(predefine::key_text_block_text_format),
        predefine::GetPredefineResourceComPtr<ID2D1Brush>(predefine::key_text_block_text_brush)
    )
    {

    }

    StringView TextBlock::GetControlType() const
    {
        return control_type;
    }
}
