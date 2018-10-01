#pragma once

#include <initializer_list>

#include "ui/control.h"
#include "border_delegate.h"

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
        void OnDraw(ID2D1DeviceContext* device_context) override;

        Size OnMeasure(const Size& available_size) override;
        void OnLayout(const Rect& rect) override;

        void OnMouseClickBegin(MouseButton button) override final;
        void OnMouseClickEnd(MouseButton button) override final;

    private:
        std::unique_ptr<BorderDelegate> border_delegate_;

        BorderProperty::Ptr normal_border_border_;
        BorderProperty::Ptr pressed_border_border_;
    };
}
