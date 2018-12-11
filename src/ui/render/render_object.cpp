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

    namespace details
    {
        template class ShapeRenderObject<Rect>;
        template class ShapeRenderObject<RoundedRect>;
        template class ShapeRenderObject<Ellipse>;
    }

    void RoundedRectangleStrokeRenderObject::Draw(ID2D1RenderTarget* render_target)
    {
        const auto brush = GetBrush();
        if (brush != nullptr)
            render_target->DrawRoundedRectangle(Convert(GetShape()), GetBrush().Get(), GetStrokeWidth());
    }

    void CustomDrawHandlerRenderObject::SetDrawHandler(DrawHandler new_draw_handler)
    {
        if (draw_handler_ == nullptr && new_draw_handler == nullptr)
            return;

        draw_handler_ = std::move(new_draw_handler);
        InvalidateRenderHost();
    }

    void CustomDrawHandlerRenderObject::Draw(ID2D1RenderTarget* render_target)
    {
        if (draw_handler_ != nullptr)
            draw_handler_(render_target);
    }
}
