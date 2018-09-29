#include "border_control.h"

namespace cru::ui::controls
{
    void BorderProperty::SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
    {
        brush_ = std::move(brush);
        RaisePropertyChangedEvent(L"Brush");
    }

    void BorderProperty::SetWidth(const float width)
    {
        width_ = width;
        RaisePropertyChangedEvent(L"Width");
    }

    void BorderProperty::SetStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style)
    {
        stroke_style_ = std::move(stroke_style);
        RaisePropertyChangedEvent(L"StrokeStyle");
    }

    void BorderProperty::SetRadiusX(const float radius_x)
    {
        radius_x_ = radius_x;
        RaisePropertyChangedEvent(L"RadiusX");
    }

    void BorderProperty::SetRadiusY(const float radius_y)
    {
        radius_y_ = radius_y;
        RaisePropertyChangedEvent(L"RadiusY");
    }
}
