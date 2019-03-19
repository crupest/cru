#include "render_object.hpp"
#include <utility>

namespace cru::ui::render
{
void RenderObject::SetRenderHost(IRenderHost* new_render_host)
{
    if (new_render_host == render_host_) return;

    const auto old = render_host_;
    render_host_ = new_render_host;
    OnRenderHostChanged(old, new_render_host);
}

void RenderObject::AddChild(RenderObject* render_object, const int position)
{
    if (render_object->GetParent() != nullptr)
        throw std::invalid_argument("Render object already has a parent.");

    if (position < 0)
        throw std::invalid_argument("Position index is less than 0.");

    if (static_cast<std::vector<RenderObject*>::size_type>(position) >
        children_.size())
        throw std::invalid_argument("Position index is out of bound.");

    children_.insert(children_.cbegin() + position, render_object);
    render_object->SetParent(this);
    OnAddChild(render_object, position);
}

void RenderObject::RemoveChild(const int position)
{
    if (position < 0)
        throw std::invalid_argument("Position index is less than 0.");

    if (static_cast<std::vector<RenderObject*>::size_type>(position) >=
        children_.size())
        throw std::invalid_argument("Position index is out of bound.");

    const auto i = children_.cbegin() + position;
    const auto removed_child = *i;
    children_.erase(i);
    removed_child->SetParent(nullptr);
    OnRemoveChild(removed_child, position);
}


void RenderObject::OnRenderHostChanged(IRenderHost* old_render_host,
                                       IRenderHost* new_render_host)
{
}

void RenderObject::InvalidateRenderHostPaint() const
{
    if (render_host_ != nullptr) render_host_->InvalidatePaint();
}

void RenderObject::InvalidateRenderHostLayout() const
{
    if (render_host_ != nullptr) render_host_->InvalidateLayout();
}

void RenderObject::OnParentChanged(RenderObject* old_parent,
                                   RenderObject* new_parent)
{
}


void RenderObject::OnAddChild(RenderObject* new_child, int position)
{
}

void RenderObject::OnRemoveChild(RenderObject* removed_child, int position)
{
}

void RenderObject::SetParent(RenderObject* new_parent)
{
    const auto old_parent = parent_;
    parent_ = new_parent;
    OnParentChanged(old_parent, new_parent);
}


void LinearLayoutRenderObject::Measure(const MeasureConstraint& constraint)
{

}
}  // namespace cru::ui::render
