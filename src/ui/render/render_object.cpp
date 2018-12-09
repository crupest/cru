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

    SingleChildRenderObject::~SingleChildRenderObject()
    {
        delete child_;
    }

    void SingleChildRenderObject::SetChild(RenderObject* new_child)
    {
        const auto old = child_;
        if (old)
            old->SetRenderHost(nullptr);
        child_ = new_child;
        if (new_child)
            new_child->SetRenderHost(GetRenderHost());
        OnChildChange(old, new_child);
        InvalidateRenderHost();
    }

    void SingleChildRenderObject::OnRenderHostChanged(IRenderHost* old_render_host, IRenderHost* new_render_host)
    {
        if (child_)
            child_->SetRenderHost(new_render_host);
    }

    void SingleChildRenderObject::OnChildChange(RenderObject* old_child, RenderObject* new_object)
    {

    }

    ClipRenderObject::ClipRenderObject(Microsoft::WRL::ComPtr<ID2D1Geometry> clip_geometry)
        : clip_geometry_(std::move(clip_geometry))
    {

    }

    void ClipRenderObject::SetClipGeometry(Microsoft::WRL::ComPtr<ID2D1Geometry> new_clip_geometry)
    {
        clip_geometry_ = std::move(new_clip_geometry);
        InvalidateRenderHost();
    }

    void ClipRenderObject::Draw(ID2D1RenderTarget* render_target)
    {
        if (clip_geometry_ != nullptr)
            render_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip_geometry_.Get()), nullptr);
        const auto child = GetChild();
        if (child != nullptr)
            child->Draw(render_target);
        if (clip_geometry_ != nullptr)
            render_target->PopLayer();
    }

    void MatrixRenderObject::ApplyAppendMatrix(ID2D1RenderTarget* render_target, const D2D1_MATRIX_3X2_F& matrix)
    {
        D2D1::Matrix3x2F old_matrix;
        render_target->GetTransform(&old_matrix);
        render_target->SetTransform(old_matrix * matrix);
    }

    void MatrixRenderObject::ApplySetMatrix(ID2D1RenderTarget* render_target, const D2D1_MATRIX_3X2_F& matrix)
    {
        render_target->SetTransform(matrix);
    }

    MatrixRenderObject::MatrixRenderObject(const D2D1_MATRIX_3X2_F& matrix, MatrixApplier applier)
        : matrix_(matrix), applier_(std::move(applier))
    {

    }

    void MatrixRenderObject::SetMatrix(const D2D1_MATRIX_3X2_F& new_matrix)
    {
        matrix_ = new_matrix;
        InvalidateRenderHost();
    }

    void MatrixRenderObject::SetMatrixApplier(MatrixApplier applier)
    {
        applier_ = std::move(applier);
        InvalidateRenderHost();
    }

    void MatrixRenderObject::Draw(ID2D1RenderTarget* render_target)
    {
        D2D1_MATRIX_3X2_F old_matrix;
        render_target->GetTransform(&old_matrix);
        applier_(render_target, matrix_);
        const auto child = GetChild();
        if (child)
            child->Draw(render_target);
        render_target->SetTransform(&old_matrix);
    }

    void OffsetRenderObject::SetOffsetX(const float new_offset_x)
    {
        offset_x_ = new_offset_x;
        SetMatrix(D2D1::Matrix3x2F::Translation(offset_x_, offset_y_));
    }

    void OffsetRenderObject::SetOffsetY(const float new_offset_y)
    {
        offset_y_ = new_offset_y;
        SetMatrix(D2D1::Matrix3x2F::Translation(offset_x_, offset_y_));
    }

    const MatrixRenderObject::MatrixApplier MatrixRenderObject::append_applier(ApplyAppendMatrix);
    const MatrixRenderObject::MatrixApplier MatrixRenderObject::set_applier(ApplySetMatrix);
}
