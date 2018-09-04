#pragma once

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
                explicit TextBlock(
                    const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format = nullptr,
                    const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush = nullptr
                );
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

                Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
                {
                    return brush_;
                }

                void SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush>& brush);

                Microsoft::WRL::ComPtr<IDWriteTextFormat> GetTextFormat() const
                {
                    return text_format_;
                }

                void SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format);

            protected:
                void OnSizeChangedCore(events::SizeChangedEventArgs& args) override final;
                void OnDraw(ID2D1DeviceContext* device_context) override;

                Size OnMeasure(const Size& available_size) override;

            private:
                void OnTextChangedCore(const String& old_text, const String& new_text);

                void CreateDefaultBrush();
                void CreateDefaultTextFormat();
                void RecreateTextLayout();

            private:
                String text_;

                Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
                Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
                Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
            };
        }
    }
}
