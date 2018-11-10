#include "text_block.hpp"

#include "ui/ui_manager.hpp"

namespace cru::ui::controls
{
    TextBlock::TextBlock() : TextControl(
        UiManager::GetInstance()->GetPredefineResources()->text_block_text_format,
        UiManager::GetInstance()->GetPredefineResources()->text_block_text_brush
    )
    {

    }

    StringView TextBlock::GetControlType() const
    {
        return control_type;
    }
}
