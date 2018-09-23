#pragma once

#include "ui/control.h"
#include "timer.h"

namespace cru::ui::controls
{
    class TextBox : public Control
    {
    protected:
        explicit TextBox(
            const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format = nullptr,
            const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush = nullptr
        );
    public:
        TextBox(const TextBox& other) = delete;
        TextBox(TextBox&& other) = delete;
        TextBox& operator=(const TextBox& other) = delete;
        TextBox& operator=(TextBox&& other) = delete;
        ~TextBox() override;

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

        void OnMouseDownCore(events::MouseButtonEventArgs& args) override final;

        void OnGetFocusCore(events::FocusChangeEventArgs& args) override;
        void OnLoseFocusCore(events::FocusChangeEventArgs& args) override final;

        Size OnMeasure(const Size& available_size) override final;

    private:
        void OnTextChangedCore(const String& old_text, const String& new_text);

        void RecreateTextLayout();

    private:
        String text_;

        Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
        Microsoft::WRL::ComPtr<ID2D1Brush> caret_brush_;
        //Microsoft::WRL::ComPtr<ID2D1Brush> selection_brush_;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
        Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;

        unsigned position_ = 0;

        TimerTask caret_timer_;
        ActionPtr caret_action_;
    };
}
