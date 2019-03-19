#pragma once

#include "pre.hpp"

#include <optional>
#include <vector>
#include "system_headers.hpp"

#include "base.hpp"
#include "ui/ui_base.hpp"

namespace cru::ui::render
{
struct MeasureConstraint
{
    std::optional<float> min_width;
    std::optional<float> min_height;
    std::optional<float> max_width;
    std::optional<float> max_height;
};


struct LayoutConstraint
{
    float preferred_width;
    float preferred_height;
};


struct IRenderHost : Interface
{
    virtual void InvalidatePaint() = 0;
    virtual void InvalidateLayout() = 0;
};


// features:
// 1. tree
// 2. layout
// 3. paint
// 3. hit test
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

    IRenderHost* GetRenderHost() const { return render_host_; }
    void SetRenderHost(IRenderHost* new_render_host);

    RenderObject* GetParent() const { return parent_; }

    const std::vector<RenderObject*>& GetChildren() const { return children_; }
    void AddChild(RenderObject* render_object, int position);
    void RemoveChild(int position);

    virtual void Measure(const MeasureConstraint& constraint) = 0;
    virtual void Layout(const LayoutConstraint& constraint) = 0;

    virtual void Draw(ID2D1RenderTarget* render_target) = 0;

    virtual void HitTest(const Point& point) = 0;

protected:
    virtual void OnRenderHostChanged(IRenderHost* old_render_host,
                                     IRenderHost* new_render_host);

    void InvalidateRenderHostPaint() const;
    void InvalidateRenderHostLayout() const;

    virtual void OnParentChanged(RenderObject* old_parent,
                                 RenderObject* new_parent);

    virtual void OnAddChild(RenderObject* new_child, int position);
    virtual void OnRemoveChild(RenderObject* removed_child, int position);

private:
    void SetParent(RenderObject* new_parent);

private:
    IRenderHost* render_host_ = nullptr;
    RenderObject* parent_ = nullptr;
    std::vector<RenderObject*> children_;
};


class LinearLayoutRenderObject : public RenderObject
{
public:
    LinearLayoutRenderObject() = default;
    LinearLayoutRenderObject(const LinearLayoutRenderObject& other) = delete;
    LinearLayoutRenderObject& operator=(const LinearLayoutRenderObject& other) =
        delete;
    LinearLayoutRenderObject(LinearLayoutRenderObject&& other) = delete;
    LinearLayoutRenderObject& operator=(LinearLayoutRenderObject&& other) =
        delete;
    ~LinearLayoutRenderObject() = default;

    void Measure(const MeasureConstraint& constraint) override;

private:
};
}  // namespace cru::ui::render
