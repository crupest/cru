#include "border_property.hpp"

#include "ui_manager.hpp"

namespace cru::ui
{
    BorderProperty::BorderProperty(): BorderProperty(UiManager::GetInstance()->GetPredefineResources()->border_property_brush)
    {

    }

    BorderProperty::BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush): brush_(std::move(brush))
    {

    }

    BorderProperty::BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush, const float width, const float radius_x,
        const float radius_y, Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style) :
        brush_(std::move(brush)), stroke_width_(width), radius_x_(radius_x), radius_y_(radius_y), stroke_style_(std::move(stroke_style))
    {

    }
}
