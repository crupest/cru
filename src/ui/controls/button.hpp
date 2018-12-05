#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include <initializer_list>

#include "ui/control.hpp"

namespace cru::ui::controls
{
    class Button : public SingleChildControl
    {
    public:
        static constexpr auto control_type = L"Button";

        static Button* Create(Control* child = nullptr)
        {
            const auto button = new Button();
            button->SetChild(child);
            return button;
        }

    protected:
        Button();

    public:
        Button(const Button& other) = delete;
        Button(Button&& other) = delete;
        Button& operator=(const Button& other) = delete;
        Button& operator=(Button&& other) = delete;
        ~Button() override = default;

        StringView GetControlType() const override final;

    protected:
        void OnMouseClickBegin(MouseButton button) override final;
        void OnMouseClickEnd(MouseButton button) override final;

    private:
        BorderProperty normal_border_;
        BorderProperty pressed_border_;
    };
}
