#pragma once
#include "../controls/Control.h"  // IWYU pragma: keep
#include "RenderObject.h"

#include <algorithm>

namespace cru::ui::render {
template <typename TChildLayoutData>
class LayoutRenderObject : public RenderObject {
 public:
  using ChildLayoutData = TChildLayoutData;

 private:
  struct ChildData {
    /**
     * May be nullptr.
     */
    RenderObject* render_object;
    ChildLayoutData layout_data;
    EventHandlerRevokerListGuard event_guard;
  };

 protected:
  LayoutRenderObject(std::string name) : RenderObject(std::move(name)) {}

  ~LayoutRenderObject() { ClearChildren(); }

 public:
  Index GetChildCount() { return static_cast<Index>(children_.size()); }

  RenderObject* GetChildAt(Index position) {
    CheckArgumentRange(position, 0, GetChildCount());
    return children_[position].render_object;
  }

  void AddChild(RenderObject* render_object, Index position) {
    CheckArgumentRange(position, 0, GetChildCount() + 1);
    auto iter = children_.insert(children_.begin() + position,
                                 ChildData{render_object, ChildLayoutData()});
    render_object->SetParent(this);
    iter->event_guard.Add(render_object->DestroyEvent()->AddSpyOnlyHandler(
        [this, render_object] { RemoveChild(render_object); }));
    InvalidateLayout();
  }

  void AddChild(RenderObject* render_object) {
    AddChild(render_object, GetChildCount());
  }

  void RemoveChild(Index position) {
    CheckArgumentRange(position, 0, GetChildCount());
    auto render_object = children_[position].render_object;
    if (render_object) {
      render_object->SetParent(nullptr);
    }
    children_.erase(children_.begin() + position);
    InvalidateLayout();
  }

  bool RemoveChild(RenderObject* render_object) {
    auto iter =
        std::ranges::find_if(children_, [render_object](const ChildData& data) {
          return data.render_object == render_object;
        });
    if (iter != children_.cend()) {
      this->RemoveChild(iter - children_.cbegin());
      return true;
    }
    return false;
  }

  void ClearChildren() {
    for (const auto& child : children_) {
      auto render_object = child.render_object;
      if (render_object) {
        render_object->SetParent(nullptr);
      }
    }
    children_.clear();
    InvalidateLayout();
  }

  const ChildLayoutData& GetChildLayoutDataAt(Index position) {
    CheckArgumentRange(position, 0, GetChildCount());
    return children_[position].layout_data;
  }

  void SetChildLayoutDataAt(Index position, ChildLayoutData data) {
    CheckArgumentRange(position, 0, GetChildCount());
    children_[position].layout_data = std::move(data);
    InvalidateLayout();
  }

  void SetChildLayoutData(RenderObject* child, ChildLayoutData data) {
    auto iter = std::find_if(
        children_.cbegin(), children_.cend(),
        [child](const ChildData& data) { return data.render_object == child; });
    if (iter != children_.cend()) {
      SetChildLayoutDataAt(iter - children_.cbegin(), std::move(data));
    }
  }

  RenderObject* HitTest(const Point& point) override {
    const auto child_count = GetChildCount();
    for (auto i = child_count - 1; i >= 0; --i) {
      const auto child = GetChildAt(i);
      const auto result = child->HitTest(point - child->GetOffset());
      if (result != nullptr) {
        return result;
      }
    }

    return GetPaddingRect().IsPointInside(point) ? this : nullptr;
  }

 protected:
  void OnDraw(RenderObjectDrawContext& context) override {
    auto painter = context.painter;
    for (const auto& child : children_) {
      context.DrawChild(child.render_object);
    }
  }

 private:
  std::vector<ChildData> children_;
};
}  // namespace cru::ui::render
