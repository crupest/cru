#pragma once
#include "RenderObject.h"

#include "cru/platform/graphics/Painter.h"

namespace cru::ui::render {
template <typename TChildLayoutData>
class LayoutRenderObject : public RenderObject {
 public:
  using ChildLayoutData = TChildLayoutData;

 private:
  struct ChildData {
    RenderObject* render_object;
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
    Expects(position >= 0 && position < GetChildCount());
    return children_[position].render_object;
  }
  void AddChild(RenderObject* render_object, Index position) {
    if (position < 0) position = 0;
    if (position > GetChildCount()) position = GetChildCount();
    children_.insert(children_.begin() + position,
                     ChildData{render_object, ChildLayoutData()});
    render_object->SetParent(this);
    InvalidateLayout();
  }

  void RemoveChild(Index position) {
    Expects(position >= 0 && position < GetChildCount());
    children_[position].render_object->SetParent(nullptr);
    children_.erase(children_.begin() + position);
    InvalidateLayout();
  }

  void ClearChildren() {
    for (auto child : children_) {
      child.render_object->SetParent(nullptr);
    }
    children_.clear();
    InvalidateLayout();
  }

  const ChildLayoutData& GetChildLayoutDataAt(Index position) const {
    Expects(position >= 0 && position < GetChildCount());
    return children_[position].layout_data;
  }

  void SetChildLayoutDataAt(Index position, ChildLayoutData data) {
    Expects(position >= 0 && position < GetChildCount());
    children_[position].layout_data = std::move(data);
    InvalidateLayout();
  }

  void Draw(platform::graphics::IPainter* painter) override {
    for (const auto& child : children_) {
      painter->PushState();
      painter->ConcatTransform(
          Matrix::Translation(child.render_object->GetOffset()));
      child.render_object->Draw(painter);
      painter->PopState();
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

 private:
  std::vector<ChildData> children_;
};
}  // namespace cru::ui::render
