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


    namespace details
    {
        template <typename TShapeType>
        class ShapeRenderObject : public virtual RenderObject
        {
        public:
            using ShapeType = TShapeType;
        protected:
            ShapeRenderObject() = default;
        public:
            ShapeRenderObject(const ShapeRenderObject& other) = delete;
            ShapeRenderObject& operator=(const ShapeRenderObject& other) = delete;
            ShapeRenderObject(ShapeRenderObject&& other) = delete;
            ShapeRenderObject& operator=(ShapeRenderObject&& other) = delete;
            ~ShapeRenderObject() override = default;

            ShapeType GetShape() const
            {
                return shape_;
            }

            void SetShape(const ShapeType& new_shape)
            {
                if (new_shape == shape_)
                    return;

                shape_ = new_shape;
                InvalidateRenderHost();
            }

        private:
            ShapeType shape_;
        };


        extern template class ShapeRenderObject<Rect>;
        extern template class ShapeRenderObject<RoundedRect>;
        extern template class ShapeRenderObject<Ellipse>;
    }


    using RectangleRenderObject = details::ShapeRenderObject<Rect>;
    using RoundedRectangleRenderObject = details::ShapeRenderObject<RoundedRect>;
    using EllipseRenderObject = details::ShapeRenderObject<Ellipse>;


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
