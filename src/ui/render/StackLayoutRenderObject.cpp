#include "cru/ui/render/StackLayoutRenderObject.h"

#include "cru/common/Logger.h"
#include "cru/ui/render/LayoutHelper.h"
#include "cru/ui/render/MeasureRequirement.h"

#include <algorithm>

namespace cru::ui::render {
void StackLayoutRenderObject::SetDefaultHorizontalAlignment(
    Alignment alignment) {
  default_horizontal_alignment_ = alignment;
  InvalidateLayout();
}

void StackLayoutRenderObject::SetDefaultVertialAlignment(Alignment alignment) {
  default_vertical_alignment_ = alignment;
  InvalidateLayout();
}

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

  for (Index i = 0; i < GetChildren().size(); ++i) {
    auto child_layout_data = GetChildLayoutData(i);
    auto horizontal_stretch =
        child_layout_data.horizontal.value_or(default_horizontal_alignment_) ==
        Alignment::Stretch;
    auto vertical_stretch =
        child_layout_data.vertical.value_or(default_vertical_alignment_) ==
        Alignment::Stretch;
    if (horizontal_stretch || vertical_stretch) {
      auto child = GetChildren()[i];
      auto child_size = child->GetSize();
      MeasureRequirement child_requirement(child_size, child_size);
      if (horizontal_stretch) {
        child_requirement.min.width = child_requirement.max.width =
            child_max_size.width;
      }

      if (vertical_stretch) {
        child_requirement.min.height = child_requirement.max.height =
            child_max_size.height;
      }
      child->Measure(child_requirement, MeasureSize::NotSpecified());
    }
  }

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
        CalculateAnchorByAlignment(
            layout_data.horizontal.value_or(default_horizontal_alignment_),
            content_rect.left, content_rect.width, size.width),
        CalculateAnchorByAlignment(
            layout_data.vertical.value_or(default_vertical_alignment_),
            content_rect.top, content_rect.height, size.height)});
  }
}
}  // namespace cru::ui::render
