#include "cru/ui/render/StackLayoutRenderObject.hpp"

#include <algorithm>

namespace cru::ui::render {
Size StackLayoutRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  auto size = Size{};
  for (const auto child : GetChildren()) {
    child->Measure(requirement);
    const auto& measure_result = child->GetMeasuredSize();
    size.width = std::max(size.width, measure_result.width);
    size.height = std::max(size.height, measure_result.height);
  }
  return size;
}

void StackLayoutRenderObject::OnLayoutContent(const Rect& rect) {
  auto calculate_anchor = [](int alignment, float start_point,
                             float total_length,
                             float content_length) -> float {
    switch (alignment) {
      case internal::align_start:
        return start_point;
      case internal::align_center:
        return start_point + (total_length - content_length) / 2.0f;
      case internal::align_end:
        return start_point + total_length - content_length;
      default:
        return start_point;
    }
  };

  const auto count = GetChildCount();
  const auto& children = GetChildren();

  for (int i = 0; i < count; i++) {
    const auto layout_data = GetChildLayoutData(i);
    const auto child = children[i];
    const auto& size = child->GetMeasuredSize();
    child->Layout(
        Rect{calculate_anchor(static_cast<int>(layout_data->horizontal),
                              rect.left, rect.width, size.width),
             calculate_anchor(static_cast<int>(layout_data->vertical), rect.top,
                              rect.height, size.height),
             size.width, size.height});
  }
}

}  // namespace cru::ui::render
