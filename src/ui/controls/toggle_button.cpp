#include "toggle_button.hpp"

#include "graph/graph.hpp"
#include "ui/animations/animation.hpp"
#include "ui/ui_manager.hpp"
#include "ui/d2d_util.hpp"

namespace cru::ui::controls
{
    using animations::AnimationBuilder;

    // ui length parameters of toggle button.
    constexpr float half_height = 15;
    constexpr float half_width = half_height * 2;
    constexpr float stroke_width = 3;
    constexpr float inner_circle_radius = half_height - stroke_width;
    constexpr float inner_circle_x = half_width - half_height;

    ToggleButton::ToggleButton() : current_circle_position_(-inner_circle_x)
    {
        graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(-half_width, -half_height, half_width, half_height), half_height, half_height), &frame_path_);

        on_brush_ = UiManager::GetInstance()->GetPredefineResources()->toggle_button_on_brush;
        off_brush_ = UiManager::GetInstance()->GetPredefineResources()->toggle_button_off_brush;

        draw_content_event.AddHandler([this](events::DrawEventArgs& args)
        {
            const auto device_context = args.GetDeviceContext();
            const auto size = GetSize();
            graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(size.width / 2, size.height / 2), [this](ID2D1DeviceContext* device_context)
            {
                if (state_)
                {
                    device_context->DrawGeometry(frame_path_.Get(), on_brush_.Get(), stroke_width);
                    device_context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(current_circle_position_, 0), inner_circle_radius, inner_circle_radius), on_brush_.Get());
                }
                else
                {
                    device_context->DrawGeometry(frame_path_.Get(), off_brush_.Get(), stroke_width);
                    device_context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(current_circle_position_, 0), inner_circle_radius, inner_circle_radius), off_brush_.Get());
                }
            });
        });

        mouse_click_event.bubble.AddHandler([this](events::MouseButtonEventArgs& args)
        {
            if (args.GetMouseButton() == MouseButton::Left)
                Toggle();
        });
    }


    StringView ToggleButton::GetControlType() const
    {
        return control_type;
    }

    bool ToggleButton::IsPointInside(const Point& point)
    {
        const auto size = GetSize();
        const auto transform = D2D1::Matrix3x2F::Translation(size.width / 2, size.height / 2);
        BOOL contains;
        frame_path_->FillContainsPoint(Convert(point), transform, &contains);
        if (!contains)
            frame_path_->StrokeContainsPoint(Convert(point), stroke_width, nullptr, transform, &contains);
        return contains != 0;
    }

    void ToggleButton::SetState(const bool state)
    {
        if (state != state_)
        {
            state_ = state;
            float destination_x;

            if (state)
                destination_x = inner_circle_x;
            else
                destination_x = -inner_circle_x;

            const auto previous_position = current_circle_position_;
            const auto delta = destination_x - current_circle_position_;

            constexpr auto total_time = FloatSecond(0.2);

            const auto time = total_time * (std::abs(delta) / (inner_circle_x * 2));

            // ReSharper disable once CppExpressionWithoutSideEffects
            AnimationBuilder(Format(L"ToggleButton {}", reinterpret_cast<size_t>(this)), time)
                .AddStepHandler([=](auto, const double percentage)
                {
                    current_circle_position_ = static_cast<float>(previous_position + delta * percentage);
                    InvalidateDraw();
                })
                .Start();

            events::ToggleEventArgs args(this, this, state);
            toggle_event.Raise(args);
            InvalidateDraw();
        }
    }

    void ToggleButton::Toggle()
    {
        SetState(!GetState());
    }

    Size ToggleButton::OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo&)
    {
        const Size result_size(
            half_width * 2 + stroke_width,
            half_height * 2 + stroke_width
        );

        return result_size;
    }
}
