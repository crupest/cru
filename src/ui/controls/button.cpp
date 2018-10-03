#include "button.h"

#include "graph/graph.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    Button::Button() : Control(true)
    {
        normal_border_ = BorderProperty::Create();
        normal_border_->SetBrush(CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue)));
        normal_border_->SetWidth(2);
        normal_border_->SetRadiusX(6);
        normal_border_->SetRadiusY(6);

        pressed_border_ = BorderProperty::Create();
        pressed_border_->SetBrush(CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Blue)));
        pressed_border_->SetWidth(2);
        pressed_border_->SetRadiusX(6);
        pressed_border_->SetRadiusY(6);

        SetBordered(true);
        SetBorderProperty(normal_border_);
    }

    void Button::OnMouseClickBegin(MouseButton button)
    {
        SetBorderProperty(pressed_border_);
        Repaint();
    }

    void Button::OnMouseClickEnd(MouseButton button)
    {
        SetBorderProperty(normal_border_);
        Repaint();
    }
}
