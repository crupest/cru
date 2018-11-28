#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include "text_control.hpp"
#include "timer.hpp"

namespace cru::ui::controls
{
    class TextBox : public TextControl
    {
    public:
        static constexpr auto control_type = L"TextBox";

        static TextBox* Create()
        {
            return new TextBox();
        }

    protected:
        TextBox();
    public:
        TextBox(const TextBox& other) = delete;
        TextBox(TextBox&& other) = delete;
        TextBox& operator=(const TextBox& other) = delete;
        TextBox& operator=(TextBox&& other) = delete;
        ~TextBox() override;

        StringView GetControlType() const override final;

    protected:
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
