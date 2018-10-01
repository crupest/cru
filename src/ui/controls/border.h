#pragma once

#include <initializer_list>

#include "ui/control.h"
#include "border_delegate.h"

namespace cru::ui::controls
{
    class Border : public Control
    {
    public:
        static Border* Create(const std::initializer_list<Control*>& children = std::initializer_list<Control*>())
        {
            const auto border = new Border();
            for (const auto control : children)
                border->AddChild(control);
            return border;
        }

    protected:
        Border();

    public:
        Border(const Border& other) = delete;
        Border(Border&& other) = delete;
        Border& operator=(const Border& other) = delete;
        Border& operator=(Border&& other) = delete;
        ~Border() override = default;

        bool IsDrawBorder() const
        {
            return draw_border_;
        }

        void SetDrawBorder(bool draw_border);

        BorderProperty::Ptr GetBorderProperty() const
        {
            return border_delegate_.GetBorderProperty();
        }

    protected:
        void OnDraw(ID2D1DeviceContext* device_context) override;

        Size OnMeasure(const Size& available_size) override;
        void OnLayout(const Rect& rect) override;

    private:
        bool draw_border_ = true;

        BorderDelegate border_delegate_;
    };
}
