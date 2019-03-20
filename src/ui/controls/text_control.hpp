#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include "ui/control.hpp"

namespace cru::ui::controls
{
    class TextControl : public NoChildControl
    {
    protected:
        TextControl(
            const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
            const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush
        );
    public:
        TextControl(const TextControl& other) = delete;
        TextControl(TextControl&& other) = delete;
        TextControl& operator=(const TextControl& other) = delete;
        TextControl& operator=(TextControl&& other) = delete;
        ~TextControl() override = default;

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

        bool IsSelectable() const
        {
            return is_selectable_;
        }

        std::optional<TextRange> GetSelectedRange() const
        {
            return selected_range_;
        }

        void SetSelectedRange(std::optional<TextRange> text_range);

        void ClearSelection()
        {
            SetSelectedRange(std::nullopt);
        }

    protected:
        void SetSelectable(bool is_selectable);


        virtual void RequestChangeCaretPosition(unsigned position);

        void OnRectChange(const Rect& old_rect, const Rect& new_rect) override;

    private:
        void OnTextChangedCore(const String& old_text, const String& new_text);

        void RecreateTextLayout();

        // param point is the mouse point relative to this control.
        void UpdateCursor(const std::optional<Point>& point);

    private:
        String text_;

        Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
        Microsoft::WRL::ComPtr<ID2D1Brush> selection_brush_;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
        Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;

        bool is_selectable_ = false;
        std::optional<TextRange> selected_range_ = std::nullopt;

        bool is_selecting_ = false;
        unsigned mouse_down_position_ = 0;
    };
}
