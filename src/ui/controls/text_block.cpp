#include "text_block.hpp"

#include "ui/window.hpp"

namespace cru::ui::controls
{
    TextBlock::TextBlock(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
        const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush) : TextControl(init_text_format, init_brush)
    {

    }

    StringView TextBlock::GetControlType() const
    {
        return control_type;
    }
}
