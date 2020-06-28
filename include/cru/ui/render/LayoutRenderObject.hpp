#pragma once
#include "RenderObject.hpp"

#include "cru/platform/graph/util/Painter.hpp"

namespace cru::ui::render {
template <typename TChildLayoutData>
class LayoutRenderObject : public RenderObject {
 public:
  using ChildLayoutData = TChildLayoutData;

 protected:
  LayoutRenderObject() : RenderObject(ChildMode::Multiple) {}

 public:
  CRU_DELETE_COPY(LayoutRenderObject)
  CRU_DELETE_MOVE(LayoutRenderObject)

  ~LayoutRenderObject() override = default;

  const std::vector<ChildLayoutData>& GetChildLayoutDataList() const {
    return this->child_layout_data_;
  }

  void SetChildLayoutData(Index position, ChildLayoutData data) {
    Expects(position >= 0 &&
            position < static_cast<Index>(this->child_layout_data_.size()));
    this->child_layout_data_[position] = std::move(data);
    this->InvalidateLayout();
  }

  const ChildLayoutData& GetChildLayoutData(Index position) const {
    Expects(position >= 0 &&
            position < static_cast<Index>(this->child_layout_data_.size()));
    return this->child_layout_data_[position];
  }

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnAddChild(RenderObject* new_child, Index position) override;
  void OnRemoveChild(RenderObject* removed_child, Index position) override;

 private:
  std::vector<ChildLayoutData> child_layout_data_{};
};

template <typename TChildLayoutData>
RenderObject* LayoutRenderObject<TChildLayoutData>::HitTest(
    const Point& point) {
  const auto& children = GetChildren();
  for (auto i = children.crbegin(); i != children.crend(); ++i) {
    auto offset = (*i)->GetOffset();
    Point p{point.x - offset.x, point.y - offset.y};
    const auto result = (*i)->HitTest(p);
    if (result != nullptr) {
      return result;
    }
  }

  const auto margin = GetMargin();
  const auto size = GetSize();
  return Rect{margin.left, margin.top,
              std::max(size.width - margin.GetHorizontalTotal(), 0.0f),
              std::max(size.height - margin.GetVerticalTotal(), 0.0f)}
                 .IsPointInside(point)
             ? this
             : nullptr;
}  // namespace cru::ui::render

template <typename TChildLayoutData>
void LayoutRenderObject<TChildLayoutData>::OnAddChild(RenderObject* new_child,
                                                      const Index position) {
  CRU_UNUSED(new_child)

  child_layout_data_.emplace(child_layout_data_.cbegin() + position);
}

template <typename TChildLayoutData>
void LayoutRenderObject<TChildLayoutData>::OnRemoveChild(
    RenderObject* removed_child, const Index position) {
  CRU_UNUSED(removed_child)

  child_layout_data_.erase(child_layout_data_.cbegin() + position);
}
}  // namespace cru::ui::render
