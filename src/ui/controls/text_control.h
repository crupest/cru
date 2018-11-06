#pragma once

#include "ui/control.h"

namespace cru::ui::controls
{
    class TextControl : public Control
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

    protected:
        void OnSizeChangedCore(events::SizeChangedEventArgs& args) override final;
        void OnDrawContent(ID2D1DeviceContext* device_context) override;

        void OnMouseDownCore(events::MouseButtonEventArgs& args) override final;
        void OnMouseMoveCore(events::MouseEventArgs& args) override final;
        void OnMouseUpCore(events::MouseButtonEventArgs& args) override final;

        void OnLoseFocusCore(events::FocusChangeEventArgs& args) override;

        Size OnMeasureContent(const Size& available_size) override;


        virtual void RequestChangeCaretPosition(unsigned position);

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
    protected:
        Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;

    private:
        bool is_selectable_ = false;

        bool is_selecting_ = false;
        unsigned mouse_down_position_ = 0;
        std::optional<TextRange> selected_range_ = std::nullopt;
    };
}
