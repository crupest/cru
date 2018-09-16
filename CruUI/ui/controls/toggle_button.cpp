#include "toggle_button.h"

#include "graph/graph.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    // ui length parameters of toggle button.
    constexpr float half_height = 15;
    constexpr float half_width = half_height * 2;
    constexpr float stroke_width = 3;
    constexpr float inner_circle_radius = half_height - stroke_width;
    constexpr float inner_circle_x = half_width - half_height;

    ToggleButton::ToggleButton()
    {
        graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(-half_width, -half_height, half_width, half_height), half_height, half_height), &frame_path_);

        on_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightBlue));
        off_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightGray));
    }

    inline D2D1_POINT_2F ConvertPoint(const Point& point)
    {
        return D2D1::Point2F(point.x, point.y);
    }

    bool ToggleButton::IsPointInside(const Point& point)
    {
        const auto size = GetSize();
        const auto transform = D2D1::Matrix3x2F::Translation(size.width / 2, size.height / 2);
        BOOL contains;
        frame_path_->FillContainsPoint(ConvertPoint(point), transform, &contains);
        if (!contains)
            frame_path_->StrokeContainsPoint(ConvertPoint(point), stroke_width, nullptr, transform, &contains);
        return contains != 0;
    }

    void ToggleButton::SetState(const bool state)
    {
        if (state != state_)
        {
            state_ = state;
            OnToggleInternal(state);
            Repaint();
        }
    }

    void ToggleButton::Toggle()
    {
        SetState(!GetState());
    }

    void ToggleButton::OnToggle(events::ToggleEventArgs& args)
    {

    }

    void ToggleButton::OnDraw(ID2D1DeviceContext* device_context)
    {
        const auto size = GetSize();
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(size.width / 2, size.height / 2), [this](ID2D1DeviceContext* device_context)
        {
            if (state_)
            {
                device_context->DrawGeometry(frame_path_.Get(), on_brush_.Get(), stroke_width);
                device_context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(inner_circle_x, 0), inner_circle_radius, inner_circle_radius), on_brush_.Get());
            }
            else
            {
                device_context->DrawGeometry(frame_path_.Get(), off_brush_.Get(), stroke_width);
                device_context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(-inner_circle_x, 0), inner_circle_radius, inner_circle_radius), off_brush_.Get());
            }
        });
    }

    void ToggleButton::OnMouseClickCore(events::MouseButtonEventArgs& args)
    {
        Control::OnMouseClickCore(args);
        Toggle();
    }

    Size ToggleButton::OnMeasure(const Size& available_size)
    {
        const auto layout_params = GetLayoutParams();

        auto&& get_measure_length = [](const MeasureLength& layout_length, const float available_length, const float fix_length) -> float
        {
            switch (layout_length.mode)
            {
            case MeasureMode::Exactly:
            {
                return std::max(std::min(layout_length.length, available_length), fix_length);
            }
            case MeasureMode::Stretch:
            {
                return std::max(available_length, fix_length);
            }
            case MeasureMode::Content:
            {
                return fix_length;
            }
            default:
                UnreachableCode();
            }
        };

        const Size result_size(
            get_measure_length(layout_params->width, available_size.width, half_width * 2 + stroke_width),
            get_measure_length(layout_params->height, available_size.height, half_height * 2 + stroke_width)
        );

        return result_size;
    }

    void ToggleButton::OnToggleInternal(bool new_state)
    {
        events::ToggleEventArgs args(this, this, new_state);
        OnToggle(args);
        toggle_event.Raise(args);
    }
}
