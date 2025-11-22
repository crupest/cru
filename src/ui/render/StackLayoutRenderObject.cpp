#include "cru/ui/render/StackLayoutRenderObject.h"

#include "cru/ui/render/LayoutHelper.h"
#include "cru/ui/render/MeasureRequirement.h"

namespace cru::ui::render {

StackLayoutRenderObject::StackLayoutRenderObject()
    : LayoutRenderObject<StackChildLayoutData>(kRenderObjectName) {}

void StackLayoutRenderObject::SetDefaultHorizontalAlignment(
    Alignment alignment) {
  default_horizontal_alignment_ = alignment;
  InvalidateLayout();
}

void StackLayoutRenderObject::SetDefaultVerticalAlignment(Alignment alignment) {
  default_vertical_alignment_ = alignment;
  InvalidateLayout();
}

Size StackLayoutRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  Size child_max_size;
  for (int i = 0; i < GetChildCount(); i++) {
    auto child = GetChildAt(i);
    child->Measure({requirement.max, MeasureSize::NotSpecified(),
                    MeasureSize::NotSpecified()});
    auto size = child->GetMeasureResultSize();
    child_max_size = child_max_size.Max(size);
  }

  child_max_size = requirement.suggest.Max(child_max_size);
  child_max_size = requirement.min.Max(child_max_size);

  for (Index i = 0; i < GetChildCount(); ++i) {
    auto child_layout_data = GetChildLayoutDataAt(i);
    auto horizontal_stretch =
        child_layout_data.horizontal.value_or(default_horizontal_alignment_) ==
        Alignment::Stretch;
    auto vertical_stretch =
        child_layout_data.vertical.value_or(default_vertical_alignment_) ==
        Alignment::Stretch;
    if (horizontal_stretch || vertical_stretch) {
      auto child = GetChildAt(i);
      auto child_size = child->GetMeasureResultSize();
      MeasureRequirement child_requirement(child_size, child_size,
                                           MeasureSize::NotSpecified());
      if (horizontal_stretch) {
        child_requirement.min.width = child_requirement.max.width =
            child_max_size.width;
      }

      if (vertical_stretch) {
        child_requirement.min.height = child_requirement.max.height =
            child_max_size.height;
      }
      child->Measure(child_requirement);
    }
  }

  return child_max_size;
}

void StackLayoutRenderObject::OnLayoutContent(const Rect& content_rect) {
  const auto count = GetChildCount();

  for (int i = 0; i < count; i++) {
    const auto child = GetChildAt(i);
    const auto& layout_data = GetChildLayoutDataAt(i);
    const auto& size = child->GetMeasureResultSize();
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
