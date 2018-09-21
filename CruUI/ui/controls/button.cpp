#include "button.h"

#include "graph/graph.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    Button::Button() : Control(true)
    {
        normal_border_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue));
        pressed_border_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::MediumBlue));
        current_border_brush_ = normal_border_brush_.Get();
    }

    void Button::OnDraw(ID2D1DeviceContext* device_context)
    {
        Control::OnDraw(device_context);
        device_context->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0, 0, GetSize().width, GetSize().height), 6, 6), current_border_brush_, 2);
    }

    void Button::OnMouseClickBegin(MouseButton button)
    {
        current_border_brush_ = pressed_border_brush_.Get();
        Repaint();
    }

    void Button::OnMouseClickEnd(MouseButton button)
    {
        current_border_brush_ = normal_border_brush_.Get();
        Repaint();
    }
}
