#include "text_block.h"

#include "ui/window.h"

namespace cru
{
    namespace ui
    {
        namespace controls
        {
            TextBlock::TextBlock(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
                const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush) : TextControl(init_text_format, init_brush)
            {

            }
        }
    }
}
