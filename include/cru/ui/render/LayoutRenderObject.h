#pragma once
#include "RenderObject.h"

#include "cru/platform/graphics/util/Painter.h"

namespace cru::ui::render {
template <typename TChildLayoutData>
class CRU_UI_API LayoutRenderObject : public RenderObject {
 public:
  using ChildLayoutData = TChildLayoutData;

 private:
  struct ChildData {
    RenderObject* child;
    ChildLayoutData layout_data;
  };

 protected:
  LayoutRenderObject() = default;

 public:
  CRU_DELETE_COPY(LayoutRenderObject)
  CRU_DELETE_MOVE(LayoutRenderObject)

  ~LayoutRenderObject() override = default;

  Index GetChildCount() const { return static_cast<Index>(children_.size()); }
  RenderObject* GetChildAt(Index position) {
    Expects(position > 0 && position < GetChildCount());
    return children_[position].render_object;
  }
  void AddChild(RenderObject* render_object, Index position) {
    if (position < 0) position = 0;
    if (position > GetChildCount()) position = GetChildCount();
    children_.insert(children_.begin() + position,
                     ChildData{render_object, ChildLayoutData()});
    render_object->SetParent(this);
  }

  void RemoveChild(Index position) {
    Expects(position > 0 && position < GetChildCount());
    children_[position].render_object->SetParent(nullptr);
    children_.erase(children_.begin() + position);
  }

  void SetChildLayoutData(Index position, ChildLayoutData data) {
    Expects(position >= 0 && position < GetChildCount());
    children_[position].layout_data = std::move(data);
    InvalidateLayout();
  }

  const ChildLayoutData& GetChildLayoutData(Index position) const {
    Expects(position >= 0 && position < GetChildCount());
    return children_[position].layout_data;
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

 private:
  std::vector<ChildData> children_;
};
}  // namespace cru::ui::render
