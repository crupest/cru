#include "border.h"

#include "graph/graph.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    Border::Border() : Control(true)
    {
        border_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black));
    }

    void Border::SetDrawBorder(bool draw_border)
    {
        draw_border_ = draw_border;
        Repaint();
    }

    void Border::SetBorderBrush(Microsoft::WRL::ComPtr<ID2D1Brush> border_brush)
    {
        border_brush_ = std::move(border_brush);
        Repaint();
    }

    void Border::SetBorderWidth(const float border_width)
    {
        border_width_ = border_width;
        Repaint();
    }

    void Border::SetBorderStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style)
    {
        border_stroke_style_ = std::move(stroke_style);
        Repaint();
    }

    void Border::SetBorderRadiusX(const float border_radius_x)
    {
        border_radius_x_ = border_radius_x;
        Repaint();
    }

    void Border::SetBorderRadiusY(const float border_radius_y)
    {
        border_radius_y_ = border_radius_y;
        Repaint();
    }

    void Border::OnDraw(ID2D1DeviceContext* device_context)
    {
        if (draw_border_)
        {
            const auto size = GetSize();
            device_context->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, size.width, size.height), border_radius_x_, border_radius_y_), border_brush_.Get(), border_width_, border_stroke_style_.Get());
        }
    }
}
