#pragma once

#include "text_control.hpp"

namespace cru::ui::controls
{
    class TextBlock : public TextControl
    {
    public:
        static constexpr auto control_type = L"TextBlock";

        static TextBlock* Create(const String& text = L"")
        {
            const auto text_block = new TextBlock();
            text_block->SetText(text);
            return text_block;
        }

    protected:
        TextBlock();
    public:
        TextBlock(const TextBlock& other) = delete;
        TextBlock(TextBlock&& other) = delete;
        TextBlock& operator=(const TextBlock& other) = delete;
        TextBlock& operator=(TextBlock&& other) = delete;
        ~TextBlock() override = default;

        StringView GetControlType() const override final;

        using TextControl::SetSelectable; // Make this public.
    };
}
