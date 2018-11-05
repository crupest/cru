#pragma once

#include "text_control.h"

namespace cru::ui::controls
{
    class TextBlock : public TextControl
    {
    public:
        static constexpr auto control_type = L"TextBlock";

        static TextBlock* Create(
            const String& text = L"",
            const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format = nullptr,
            const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush = nullptr)
        {
            const auto text_block = new TextBlock(init_text_format, init_brush);
            text_block->SetText(text);
            return text_block;
        }

    protected:
        TextBlock(
            const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
            const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush
        );
    public:
        TextBlock(const TextBlock& other) = delete;
        TextBlock(TextBlock&& other) = delete;
        TextBlock& operator=(const TextBlock& other) = delete;
        TextBlock& operator=(TextBlock&& other) = delete;
        ~TextBlock() override = default;

        StringView GetControlType() const override final;
    };
}
