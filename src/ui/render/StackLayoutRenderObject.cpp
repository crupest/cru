#include "cru/ui/render/StackLayoutRenderObject.hpp"

#include "cru/common/Logger.hpp"
#include "cru/ui/render/LayoutHelper.hpp"

#include <algorithm>

namespace cru::ui::render {
Size StackLayoutRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement, const MeasureSize& preferred_size) {
  Size max_size;
  for (const auto child : GetChildren()) {
    child->Measure(
        MeasureRequirement{
            MeasureSize{StackLayoutCalculateChildMaxLength(
                            preferred_size.width, requirement.max.width,
                            child->GetMinSize().width,
                            "StackLayoutRenderObject: Child's min width is "
                            "bigger than parent's max width."),
                        StackLayoutCalculateChildMaxLength(
                            preferred_size.height, requirement.max.height,
                            child->GetMinSize().height,
                            "StackLayoutRenderObject: Child's min height is "
                            "bigger than parent's max height.")},
            MeasureSize::NotSpecified()},
        MeasureSize::NotSpecified());
    const auto size = child->GetSize();
    max_size.width = std::max(max_size.width, size.width);
    max_size.height = std::max(max_size.height, size.height);
  }

  max_size = Max(preferred_size.GetSizeOr0(), max_size);
  max_size = Max(requirement.min.GetSizeOr0(), max_size);

  return max_size;
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
