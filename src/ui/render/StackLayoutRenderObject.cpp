#include "cru/ui/render/StackLayoutRenderObject.hpp"

#include "cru/common/Logger.hpp"
#include "cru/ui/render/LayoutHelper.hpp"
#include "cru/ui/render/MeasureRequirement.hpp"

#include <algorithm>

namespace cru::ui::render {
Size StackLayoutRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement, const MeasureSize& preferred_size) {
  Size child_max_size;
  for (const auto child : GetChildren()) {
    child->Measure(
        MeasureRequirement(requirement.max, MeasureSize::NotSpecified()),
        MeasureSize::NotSpecified());
    const auto size = child->GetSize();
    child_max_size.width = std::max(child_max_size.width, size.width);
    child_max_size.height = std::max(child_max_size.height, size.height);
  }

  child_max_size = Max(preferred_size.GetSizeOr0(), child_max_size);
  child_max_size = Max(requirement.min.GetSizeOr0(), child_max_size);

  return child_max_size;
}

void StackLayoutRenderObject::OnLayoutContent(const Rect& content_rect) {
  const auto count = GetChildCount();
  const auto& children = GetChildren();

  for (int i = 0; i < count; i++) {
    const auto child = children[i];
    const auto& layout_data = GetChildLayoutData(i);
    const auto& size = child->GetSize();
    child->Layout(Point{
        CalculateAnchorByAlignment(layout_data.horizontal, content_rect.left,
                                   content_rect.width, size.width),
        CalculateAnchorByAlignment(layout_data.vertical, content_rect.top,
                                   content_rect.height, size.height)});
  }
}
}  // namespace cru::ui::render
