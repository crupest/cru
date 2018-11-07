#include "button.hpp"

#include "graph/graph.hpp"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    Button::Button() : Control(true),
        normal_border_{CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::RoyalBlue))},
        pressed_border_{CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Blue))}
    {
        normal_border_.SetStrokeWidth(2);
        normal_border_.SetRadiusX(6);
        normal_border_.SetRadiusY(6);

        pressed_border_.SetStrokeWidth(2);
        pressed_border_.SetRadiusX(6);
        pressed_border_.SetRadiusY(6);

        SetBordered(true);
        GetBorderProperty() = normal_border_;

        SetCursor(cursors::hand);
    }

    StringView Button::GetControlType() const
    {
        return control_type;
    }

    void Button::OnMouseClickBegin(MouseButton button)
    {
        GetBorderProperty() = pressed_border_;
        InvalidateBorder();
    }

    void Button::OnMouseClickEnd(MouseButton button)
    {
        GetBorderProperty() = normal_border_;
        InvalidateBorder();
    }
}
