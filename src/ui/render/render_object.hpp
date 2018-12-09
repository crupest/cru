#pragma once

#include "pre.hpp"

#include "system_headers.hpp"
#include <functional>

#include "base.hpp"
#include "ui/ui_base.hpp"

namespace cru::ui::render
{
    /* About Render Object
     * 
     * Render object is a concrete subclass of RenderObject class.
     * It represents a painting action on a d2d render target. By
     * overriding "Draw" virtual method, it can customize its painting
     * action.
     * 
     * Render object may have implicit children to form a tree.
     * RenderObject class doesn't provide any method to access children.
     * Instead any concrete render object manage their own child model
     * and interface and optionally call Draw on children so when root
     * call Draw descendants' Draw will be called recursively.
     * 
     * Related render objects of a control are a subtree of the whole tree.
     * So render objects can be composed easily to form a whole control.
     * 
     * Render object may do no actual drawing but perform particular
     * actions on render target. Like ClipRenderObject apply a clip on
     * render target, MatrixRenderObject apply a matrix on render
     * target.
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


    // It is subclass duty to call child's Draw.
    class SingleChildRenderObject : public RenderObject
    {
    protected:
        SingleChildRenderObject() = default;
    public:
        SingleChildRenderObject(const SingleChildRenderObject& other) = delete;
        SingleChildRenderObject(SingleChildRenderObject&& other) = delete;
        SingleChildRenderObject& operator=(const SingleChildRenderObject& other) = delete;
        SingleChildRenderObject& operator=(SingleChildRenderObject&& other) = delete;
        ~SingleChildRenderObject();

        RenderObject* GetChild() const
        {
            return child_;
        }

        void SetChild(RenderObject* new_child);

    protected:
        void OnRenderHostChanged(IRenderHost* old_render_host, IRenderHost* new_render_host) override;

        virtual void OnChildChange(RenderObject* old_child, RenderObject* new_object);

    private:
        RenderObject* child_ = nullptr;
    };


    class ClipRenderObject final : public SingleChildRenderObject
    {
    public:
        explicit ClipRenderObject(Microsoft::WRL::ComPtr<ID2D1Geometry> clip_geometry = nullptr);
        ClipRenderObject(const ClipRenderObject& other) = delete;
        ClipRenderObject(ClipRenderObject&& other) = delete;
        ClipRenderObject& operator=(const ClipRenderObject& other) = delete;
        ClipRenderObject& operator=(ClipRenderObject&& other) = delete;
        ~ClipRenderObject() = default;

        Microsoft::WRL::ComPtr<ID2D1Geometry> GetClipGeometry() const
        {
            return clip_geometry_;
        }
        void SetClipGeometry(Microsoft::WRL::ComPtr<ID2D1Geometry> new_clip_geometry);

        void Draw(ID2D1RenderTarget* render_target) override final;

    private:
        Microsoft::WRL::ComPtr<ID2D1Geometry> clip_geometry_;
    };


    class MatrixRenderObject: public SingleChildRenderObject
    {
    private:
        static void ApplyAppendMatrix(ID2D1RenderTarget* render_target, const D2D1_MATRIX_3X2_F& matrix);
        static void ApplySetMatrix(ID2D1RenderTarget* render_target, const D2D1_MATRIX_3X2_F& matrix);

    public:
        using MatrixApplier = std::function<void(ID2D1RenderTarget*, const D2D1_MATRIX_3X2_F&)>;

        static const MatrixApplier append_applier;
        static const MatrixApplier set_applier;

        explicit MatrixRenderObject(const D2D1_MATRIX_3X2_F& matrix = D2D1::Matrix3x2F::Identity(),
                                    MatrixApplier applier = append_applier);
        MatrixRenderObject(const MatrixRenderObject& other) = delete;
        MatrixRenderObject(MatrixRenderObject&& other) = delete;
        MatrixRenderObject& operator=(const MatrixRenderObject& other) = delete;
        MatrixRenderObject& operator=(MatrixRenderObject&& other) = delete;
        ~MatrixRenderObject() = default;

        D2D1_MATRIX_3X2_F GetMatrix() const
        {
            return matrix_;
        }

        void SetMatrix(const D2D1_MATRIX_3X2_F& new_matrix);

        MatrixApplier GetMatrixApplier() const
        {
            return applier_;
        }

        void SetMatrixApplier(MatrixApplier applier);

        void Draw(ID2D1RenderTarget* render_target) override final;

    private:
        D2D1_MATRIX_3X2_F matrix_;
        MatrixApplier applier_;
    };


    class OffsetRenderObject : public MatrixRenderObject
    {
    public:
        explicit OffsetRenderObject(const float offset_x = 0.0f, const float offset_y = 0.0f)
            : MatrixRenderObject(D2D1::Matrix3x2F::Translation(offset_x, offset_y)),
            offset_x_(offset_x), offset_y_(offset_y)
        {

        }

        float GetOffsetX() const
        {
            return offset_x_;
        }

        void SetOffsetX(float new_offset_x);

        float GetOffsetY() const
        {
            return offset_y_;
        }

        void SetOffsetY(float new_offset_y);

    private:
        float offset_x_;
        float offset_y_;
    };

    class BorderRenderObject; //TODO!

    class FillGeometryRenderObject; //TODO!

    class CustomDrawHandlerRenderObject; //TODO!

    class ContainerRenderObject; //TODO!



    // Render object tree for a control. (RO for RenderObject)
    //
    //                                        ControlRO (not a SingleChildRO because child is not changed)
    //                                             |
    //                                         MatrixRO (control transform, only matrix exposed)
    //                                             |
    //                                           ClipRO (control clip, only clip geometry exposed)
    //                                             |
    //                                         OffsetRO (border offset)
    //                                             |
    //                                        ContainerRO
    //                                     /       |
    //                             BorderRO    OffsetRO (padding offset)
    //                                        /    |     \
    //                                     /       |        \
    //                                  /          |           \
    //                               /             |              \
    //                            /                |                 \
    //                         /                   |                    \
    //                ContainerRO (background)     |         ContainerRO (foreground, symmetrical to background)
    //                  /      \                   |                  /           \
    //   GeometryFillRO    CustomDrawHandlerRO     |        GeometryFillRO      CustomDrawHandlerRO
    //                                             |
    //                                         OffsetRO (content offset)
    //                                             |
    //                                       ContainerRO (content)
    //                                    /        |        \
    //                                 /           |           \
    //                              /              |              \
    //   ContainerRO (control-define content ROs)  |            ContainerRO (child-control ROs)
    //                                             |
    //                                     CustomDrawHandlerRO (user-define drawing)
    //
    class ControlRenderObject : public RenderObject
    {
    public:
        ControlRenderObject() = default;
        ControlRenderObject(const ControlRenderObject& other) = delete;
        ControlRenderObject(ControlRenderObject&& other) = delete;
        ControlRenderObject& operator=(const ControlRenderObject& other) = delete;
        ControlRenderObject& operator=(ControlRenderObject&& other) = delete;
        ~ControlRenderObject() override = default;


        D2D1_MATRIX_3X2_F GetControlTransform() const;
        Microsoft::WRL::ComPtr<ID2D1Geometry> GetControlClip() const;

        Point GetBorderOffset() const;
        BorderRenderObject* GetBorderRenderObject() const;

        Point GetPaddingOffset() const;
        Microsoft::WRL::ComPtr<ID2D1Geometry> GetPaddingGeometry() const;

        Point GetContentOffset() const;
        ContainerRenderObject* GetContentContainer() const;

        ContainerRenderObject* GetChildrenContainer() const;
    };
}
