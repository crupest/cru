#pragma once

#include "ui/ui_base.h"
#include "ui/control.h"

namespace cru
{
    namespace ui
    {
        namespace controls
        {
            class TextBlock : public Control
            {
            public:
                TextBlock();
                TextBlock(const TextBlock& other) = delete;
                TextBlock(TextBlock&& other) = delete;
                TextBlock& operator=(const TextBlock& other) = delete;
                TextBlock& operator=(TextBlock&& other) = delete;
                ~TextBlock() override;

                String GetText() const
                {
                    return text_;
                }

                void SetText(const String& text);

            protected:
                void OnSizeChangedCore(events::SizeChangedEventArgs& args) override final;

            private:
                void OnTextChangedCore(const String& old_text, const String& new_text);

            private:
                String text_;
            };
        }
    }
}
