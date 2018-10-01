#include "button.h"

#include "graph/graph.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    Button::Button() : Control(true)
    {
        normal_border_border_ = BorderProperty::Create();
        normal_border_border_->SetBrush(CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue)));
        normal_border_border_->SetWidth(2);
        normal_border_border_->SetRadiusX(6);
        normal_border_border_->SetRadiusY(6);

        pressed_border_border_ = BorderProperty::Create();
        pressed_border_border_->SetBrush(CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Blue)));
        pressed_border_border_->SetWidth(2);
        pressed_border_border_->SetRadiusX(6);
        pressed_border_border_->SetRadiusY(6);

        border_delegate_ = std::make_unique<BorderDelegate>(this, normal_border_border_);
    }

    void Button::OnDraw(ID2D1DeviceContext* device_context)
    {
        Control::OnDraw(device_context);
        border_delegate_->Draw(device_context, GetSize());
    }

    Size Button::OnMeasure(const Size& available_size)
    {
        return Control::DefaultMeasureWithPadding(available_size, border_delegate_->GetBorderThickness());
    }

    void Button::OnLayout(const Rect& rect)
    {
        Control::DefaultLayoutWithPadding(rect, border_delegate_->GetBorderThickness());
    }

    void Button::OnMouseClickBegin(MouseButton button)
    {
        border_delegate_->SetBorderProperty(pressed_border_border_);
        Repaint();
    }

    void Button::OnMouseClickEnd(MouseButton button)
    {
        border_delegate_->SetBorderProperty(normal_border_border_);
        Repaint();
    }
}
