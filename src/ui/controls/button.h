#pragma once

#include <initializer_list>

#include "ui/control.h"

namespace cru::ui::controls
{
    class Button : public Control
    {
    public:
        static Button* Create(const std::initializer_list<Control*>& children = std::initializer_list<Control*>())
        {
            const auto button = new Button();
            for (const auto control : children)
                button->AddChild(control);
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

    protected:
        void OnMouseClickBegin(MouseButton button) override final;
        void OnMouseClickEnd(MouseButton button) override final;

    private:
        BorderProperty::Ptr normal_border_;
        BorderProperty::Ptr pressed_border_;
    };
}
