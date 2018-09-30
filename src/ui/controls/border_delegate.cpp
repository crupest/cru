#include "border_delegate.h"

namespace cru::ui::controls
{
    void BorderProperty::SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
    {
        brush_ = std::move(brush);
        RaisePropertyChangedEvent(brush_property_name);
    }

    void BorderProperty::SetWidth(const float width)
    {
        width_ = width;
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

    BorderDelegate::BorderDelegate(Control* control)
        : BorderDelegate(control, std::make_shared<BorderProperty>())
    {

    }

    BorderDelegate::BorderDelegate(Control* control, std::shared_ptr<BorderProperty> border_property)\
        : control_(control),
        border_property_changed_listener_(CreateFunctionPtr<void(String)>([this](String property_name)
    {
        if (property_name == BorderProperty::width_property_name)
            control_->InvalidateLayout();
        control_->Repaint();
    }))
    {
        border_property_ = std::move(border_property);
        border_property_->AddPropertyChangedListener(border_property_changed_listener_);
    }

    void BorderDelegate::SetBorderProperty(std::shared_ptr<BorderProperty> border_property)
    {
        border_property_->RemovePropertyChangedListener(border_property_changed_listener_);
        border_property_ = std::move(border_property);
        border_property_->AddPropertyChangedListener(border_property_changed_listener_);
    }

    void BorderDelegate::Draw(ID2D1DeviceContext* device_context, const Size& size) const
    {
        device_context->DrawRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, size.width, size.height),
                border_property_->GetRadiusX(),
                border_property_->GetRadiusY()
            ),
            border_property_->GetBrush().Get(),
            border_property_->GetWidth(),
            border_property_->GetStrokeStyle().Get()
        );
    }

    Size BorderDelegate::GetBorderSize() const
    {
        const auto width = border_property_->GetWidth();
        return Size(width * 2, width * 2);
    }

    Rect BorderDelegate::CoerceLayoutRect(const Rect& rect) const
    {
        const auto width = border_property_->GetWidth();
        return Rect(rect.left + width, rect.top + width, rect.width - width * 2, rect.height - width * 2);
    }
}
