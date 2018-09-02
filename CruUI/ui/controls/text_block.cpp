#include "text_block.h"

namespace cru
{
    namespace ui
    {
        namespace controls
        {
            void TextBlock::SetText(const String& text)
            {
                const auto old_text = text_;
                text_ = text;
                OnTextChangedCore(old_text, text);
            }

            void TextBlock::OnSizeChangedCore(events::SizeChangedEventArgs& args)
            {

            }

            void TextBlock::OnTextChangedCore(const String& old_text, const String& new_text)
            {

            }
        }
    }
}
