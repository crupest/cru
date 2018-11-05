#pragma once

#include "text_control.h"
#include "timer.h"

namespace cru::ui::controls
{
    class TextBox : public TextControl
    {
    public:
        static TextBox* Create(
            const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format = nullptr,
            const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush = nullptr)
        {
            return new TextBox(init_text_format, init_brush);
        }

    protected:
        explicit TextBox(
            const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
            const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush
        );
    public:
        TextBox(const TextBox& other) = delete;
        TextBox(TextBox&& other) = delete;
        TextBox& operator=(const TextBox& other) = delete;
        TextBox& operator=(TextBox&& other) = delete;
        ~TextBox() override;

    protected:
        void OnDrawContent(ID2D1DeviceContext* device_context) override;

        void OnGetFocusCore(events::FocusChangeEventArgs& args) override final;
        void OnLoseFocusCore(events::FocusChangeEventArgs& args) override final;

        void OnKeyDownCore(events::KeyEventArgs& args) override final;
        void OnCharCore(events::CharEventArgs& args) override final;

        void RequestChangeCaretPosition(unsigned position) override final;

    private:
        // return true if left
        bool GetCaretSelectionSide() const;
        void ShiftLeftSelectionRange(int count);
        void ShiftRightSelectionRange(int count);

    private:
        unsigned caret_position_ = 0;
        std::optional<TimerTask> caret_timer_{};
        Microsoft::WRL::ComPtr<ID2D1Brush> caret_brush_;
        bool is_caret_show_ = false;
    };
}
