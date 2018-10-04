#include "border_property.h"

#include "graph/graph.h"

namespace cru::ui
{
    BorderProperty::BorderProperty()
    {
        brush_ = graph::CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black));
    }

    void BorderProperty::SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
    {
        if (brush == nullptr)
            throw std::invalid_argument("Brush of BorderProperty mustn't be null.");
        brush_ = std::move(brush);
        RaisePropertyChangedEvent(brush_property_name);
    }

    void BorderProperty::SetWidth(const float width)
    {
        stroke_width_ = width;
        RaisePropertyChangedEvent(width_property_name);
    }

    void BorderProperty::SetStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style)
    {
        stroke_style_ = std::move(stroke_style);
        RaisePropertyChangedEvent(stroke_style_property_name);
    }

    void BorderProperty::SetRadiusX(const float radius_x)
    {
        radius_x_ = radius_x;
        RaisePropertyChangedEvent(radius_x_property_name);
    }

    void BorderProperty::SetRadiusY(const float radius_y)
    {
        radius_y_ = radius_y;
        RaisePropertyChangedEvent(radius_y_property_name);
    }
}
