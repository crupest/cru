#pragma once
#include "render_object.hpp"

#include "cru/platform/graph/util/painter.hpp"

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

  ChildLayoutData* GetChildLayoutData(Index position) {
    Expects(position >= 0 &&
            position < static_cast<Index>(child_layout_data_.size()));
    return &child_layout_data_[position];
  }

  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnAddChild(RenderObject* new_child, Index position) override;
  void OnRemoveChild(RenderObject* removed_child, Index position) override;

 private:
  std::vector<ChildLayoutData> child_layout_data_{};
};

template <typename TChildLayoutData>
void LayoutRenderObject<TChildLayoutData>::Draw(
    platform::graph::IPainter* painter) {
  for (const auto child : GetChildren()) {
    auto offset = child->GetOffset();
    platform::graph::util::WithTransform(
        painter, platform::Matrix::Translation(offset.x, offset.y),
        [child](auto p) { child->Draw(p); });
  }
}

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
