#include "border_property.hpp"

#include "predefine.hpp"

namespace cru::ui
{
    BorderProperty::BorderProperty(): BorderProperty(predefine::GetPredefineResourceComPtr<ID2D1Brush>(predefine::key_border_property_brush))
    {

    }

    BorderProperty::BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush): brush_(std::move(brush))
    {

    }
}
