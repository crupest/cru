#pragma once

#include "pre.hpp"

#include "system_headers.hpp"
#include <functional>
#include <cassert>

#include "base.hpp"
#include "ui/ui_base.hpp"
#include "ui/d2d_util.hpp"

namespace cru::ui::render
{
    /* About Render Object
     * 
     * Render object is a concrete subclass of RenderObject class.
     * It represents a painting action on a d2d render target. By
     * overriding "Draw" virtual method, it can customize its painting
     * action.
     */


    struct IRenderHost : Interface
    {
        virtual void InvalidateRender() = 0;
    };


    class RenderObject : public Object
    {
    protected:
        RenderObject() = default;
    public:
        RenderObject(const RenderObject& other) = delete;
        RenderObject(RenderObject&& other) = delete;
        RenderObject& operator=(const RenderObject& other) = delete;
        RenderObject& operator=(RenderObject&& other) = delete;
        ~RenderObject() override = default;

        virtual void Draw(ID2D1RenderTarget* render_target) = 0;

        IRenderHost* GetRenderHost() const
        {
            return render_host_;
        }

        void SetRenderHost(IRenderHost* new_render_host);

    protected:
        virtual void OnRenderHostChanged(IRenderHost* old_render_host, IRenderHost* new_render_host);

        void InvalidateRenderHost();

    private:
        IRenderHost* render_host_ = nullptr;
    };


    class StrokeRenderObject : public virtual RenderObject
    {
    protected:
        StrokeRenderObject() = default;
    public:
        StrokeRenderObject(const StrokeRenderObject& other) = delete;
        StrokeRenderObject(StrokeRenderObject&& other) = delete;
        StrokeRenderObject& operator=(const StrokeRenderObject& other) = delete;
        StrokeRenderObject& operator=(StrokeRenderObject&& other) = delete;
        ~StrokeRenderObject() override = default;

        float GetStrokeWidth() const
        {
            return stroke_width_;
        }

        void SetStrokeWidth(float new_stroke_width);

        Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
        {
            return brush_;
        }

        void SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> new_brush);

        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> GetStrokeStyle() const
        {
            return stroke_style_;
        }

        void SetStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> new_stroke_style);

    private:
        float stroke_width_ = 1.0f;
        Microsoft::WRL::ComPtr<ID2D1Brush> brush_ = nullptr;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style_ = nullptr;
    };


    class FillRenderObject : public virtual RenderObject
    {
    protected:
        FillRenderObject() = default;
    public:
        FillRenderObject(const FillRenderObject& other) = delete;
        FillRenderObject(FillRenderObject&& other) = delete;
        FillRenderObject& operator=(const FillRenderObject& other) = delete;
        FillRenderObject& operator=(FillRenderObject&& other) = delete;
        ~FillRenderObject() override = default;

        Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
        {
            return brush_;
        }

        void SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> new_brush);

    private:
        Microsoft::WRL::ComPtr<ID2D1Brush> brush_ = nullptr;
    };


    class RoundedRectangleRenderObject : public virtual RenderObject
    {
    protected:
        RoundedRectangleRenderObject() = default;
    public:
        RoundedRectangleRenderObject(const RoundedRectangleRenderObject& other) = delete;
        RoundedRectangleRenderObject(RoundedRectangleRenderObject&& other) = delete;
        RoundedRectangleRenderObject& operator=(const RoundedRectangleRenderObject& other) = delete;
        RoundedRectangleRenderObject& operator=(RoundedRectangleRenderObject&& other) = delete;
        ~RoundedRectangleRenderObject() override = default;

        Rect GetRect() const
        {
            return Convert(rounded_rect_.rect);
        }

        void SetRect(const Rect& rect);

        float GetRadiusX() const
        {
            return rounded_rect_.radiusX;
        }

        void SetRadiusX(float new_radius_x);

        float GetRadiusY() const
        {
            return rounded_rect_.radiusY;
        }

        void SetRadiusY(float new_radius_y);

        D2D1_ROUNDED_RECT GetRoundedRect() const
        {
            return rounded_rect_;
        }

        void SetRoundedRect(const D2D1_ROUNDED_RECT& new_rounded_rect);

    private:
        D2D1_ROUNDED_RECT rounded_rect_ = D2D1::RoundedRect(D2D1::RectF(), 0.0f, 0.0f);
    };


    class RoundedRectangleStrokeRenderObject final : public StrokeRenderObject, public RoundedRectangleRenderObject
    {
    public:
        RoundedRectangleStrokeRenderObject() = default;
        RoundedRectangleStrokeRenderObject(const RoundedRectangleStrokeRenderObject& other) = delete;
        RoundedRectangleStrokeRenderObject(RoundedRectangleStrokeRenderObject&& other) = delete;
        RoundedRectangleStrokeRenderObject& operator=(const RoundedRectangleStrokeRenderObject& other) = delete;
        RoundedRectangleStrokeRenderObject& operator=(RoundedRectangleStrokeRenderObject&& other) = delete;
        ~RoundedRectangleStrokeRenderObject() override = default;

    protected:
        void Draw(ID2D1RenderTarget* render_target) override;
    };


    class CustomDrawHandlerRenderObject : public RenderObject
    {
    public:
        using DrawHandler = std::function<void(ID2D1RenderTarget*)>;

        CustomDrawHandlerRenderObject() = default;
        CustomDrawHandlerRenderObject(const CustomDrawHandlerRenderObject& other) = delete;
        CustomDrawHandlerRenderObject& operator=(const CustomDrawHandlerRenderObject& other) = delete;
        CustomDrawHandlerRenderObject(CustomDrawHandlerRenderObject&& other) = delete;
        CustomDrawHandlerRenderObject& operator=(CustomDrawHandlerRenderObject&& other) = delete;
        ~CustomDrawHandlerRenderObject() override = default;

        DrawHandler GetDrawHandler() const
        {
            return draw_handler_;
        }

        void SetDrawHandler(DrawHandler new_draw_handler);

    protected:
        void Draw(ID2D1RenderTarget* render_target) override;

    private:
        DrawHandler draw_handler_{};
    };
}
