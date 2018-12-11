#include "render_object.hpp"
#include <utility>

namespace cru::ui::render
{
    void RenderObject::SetRenderHost(IRenderHost* new_render_host)
    {
        if (new_render_host == render_host_)
            return;

        const auto old = render_host_;
        render_host_ = new_render_host;
        OnRenderHostChanged(old, new_render_host);
    }

    void RenderObject::OnRenderHostChanged(IRenderHost* old_render_host, IRenderHost* new_render_host)
    {

    }

    void RenderObject::InvalidateRenderHost()
    {
        if (render_host_ != nullptr)
            render_host_->InvalidateRender();
    }

    void StrokeRenderObject::SetStrokeWidth(const float new_stroke_width)
    {
        if (stroke_width_ == new_stroke_width)
            return;

        stroke_width_ = new_stroke_width;
        InvalidateRenderHost();
    }

    void StrokeRenderObject::SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> new_brush)
    {
        if (brush_ == new_brush)
            return;

        brush_ = std::move(new_brush);
        InvalidateRenderHost();
    }

    void StrokeRenderObject::SetStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> new_stroke_style)
    {
        if (stroke_style_ == new_stroke_style)
            return;

        stroke_style_ = std::move(new_stroke_style);
        InvalidateRenderHost();
    }

    void FillRenderObject::SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> new_brush)
    {
        if (brush_ == new_brush)
            return;

        brush_ = std::move(new_brush);
        InvalidateRenderHost();
    }

    void RoundedRectangleRenderObject::SetRect(const Rect& rect)
    {
        const auto converted_rect = Convert(rect);
        if (rounded_rect_.rect == converted_rect)
            return;

        rounded_rect_.rect = converted_rect;
        InvalidateRenderHost();
    }

    void RoundedRectangleRenderObject::SetRadiusX(const float new_radius_x)
    {
        if (rounded_rect_.radiusX == new_radius_x)
            return;

        rounded_rect_.radiusX = new_radius_x;
        InvalidateRenderHost();
    }

    void RoundedRectangleRenderObject::SetRadiusY(const float new_radius_y)
    {
        if (rounded_rect_.radiusY == new_radius_y)
            return;

        rounded_rect_.radiusY = new_radius_y;
        InvalidateRenderHost();
    }

    void RoundedRectangleRenderObject::SetRoundedRect(const D2D1_ROUNDED_RECT& new_rounded_rect)
    {
        if (rounded_rect_ == new_rounded_rect)
            return;

        rounded_rect_ = new_rounded_rect;
        InvalidateRenderHost();
    }

    void RoundedRectangleStrokeRenderObject::Draw(ID2D1RenderTarget* render_target)
    {
        const auto brush = GetBrush();
        if (brush != nullptr)
            render_target->DrawRoundedRectangle(GetRoundedRect(), GetBrush().Get(), GetStrokeWidth());
    }

    void CustomDrawHandlerRenderObject::SetDrawHandler(DrawHandler new_draw_handler)
    {
        if (draw_handler_ == nullptr && new_draw_handler == nullptr)
            return;

        draw_handler_ = std::move(new_draw_handler);
        InvalidateRenderHost;
    }

    void CustomDrawHandlerRenderObject::Draw(ID2D1RenderTarget* render_target)
    {
        if (draw_handler_ != nullptr)
            draw_handler_(render_target);
    }
}
