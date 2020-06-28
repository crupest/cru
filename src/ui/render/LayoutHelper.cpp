#include "cru/ui/render/LayoutHelper.hpp"

#include "cru/common/Logger.hpp"

namespace cru::ui::render {
float CalculateAnchorByAlignment(Alignment alignment, float start_point,
                                 float content_length, float child_length) {
  switch (alignment) {
    case FlexCrossAlignment::Start:
      return start_point;
    case FlexCrossAlignment::Center:
      return start_point + (content_length - child_length) / 2.0f;
    case FlexCrossAlignment::End:
      return start_point + content_length - child_length;
    default:
      return start_point;
  }
}

MeasureLength StackLayoutCalculateChildMaxLength(
    MeasureLength parent_preferred_size, MeasureLength parent_max_size,
    MeasureLength child_min_size, std::string_view log_tag,
    std::string_view exceeds_message) {
  if (parent_max_size.GetLengthOrMax() < child_min_size.GetLengthOr0()) {
    log::TagWarn(log_tag, exceeds_message);
    return parent_max_size;
  }

  if (parent_preferred_size.IsSpecified() &&
      parent_preferred_size.GetLengthOrUndefined() >=
          child_min_size.GetLengthOr0()) {
    return parent_preferred_size;
  } else {
    return parent_max_size;
  }
}
}  // namespace cru::ui::render
