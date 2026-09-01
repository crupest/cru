#pragma once
#include "../controls/Control.h"  // IWYU pragma: keep
#include "RenderObject.h"

#include <algorithm>

namespace cru::ui::render {
template <typename TChildLayoutData>
class LayoutRenderObject : public RenderObject {
 public:
  using ChildLayoutData = TChildLayoutData;

  using RenderObject::AddChild;
  using RenderObject::InsertChildAt;
  using RenderObject::RemoveChildAt;

 protected:
  LayoutRenderObject(std::string name) : RenderObject(std::move(name)) {}

 public:
  const ChildLayoutData& GetChildLayoutDataAt(Index position) {
    CheckArgumentRange(position, 0, GetChildCount());
    return layout_data_[position];
  }

  void SetChildLayoutDataAt(Index position, ChildLayoutData data) {
    CheckArgumentRange(position, 0, GetChildCount());
    layout_data_[position] = std::move(data);
    InvalidateLayout();
  }

  void SetChildLayoutData(RenderObject* child, ChildLayoutData data) {
    auto index = IndexOfChild(child);
    if (index != -1) {
      SetChildLayoutDataAt(index, std::move(data));
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
    for (const auto& child : GetChildren()) {
      context.DrawChild(child);
    }
  }

  void OnChildInserted(RenderObject* child, Index index) override {
    layout_data_.emplace(layout_data_.cbegin() + index);
    RenderObject::OnChildInserted(child, index);
  }

  void OnChildRemoved(RenderObject* child, Index index) override {
    layout_data_.erase(layout_data_.cbegin() + index);
    RenderObject::OnChildRemoved(child, index);
  }

 private:
  std::vector<ChildLayoutData> layout_data_;
};
}  // namespace cru::ui::render
