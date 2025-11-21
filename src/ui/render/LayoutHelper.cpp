#include "cru/ui/render/LayoutHelper.h"

namespace cru::ui::render {
float CalculateAnchorByAlignment(Alignment alignment, float start_point,
                                 float content_length, float child_length) {
  switch (alignment) {
    case Alignment::Start:
      return start_point;
    case Alignment::Center:
      return start_point + (content_length - child_length) / 2.0f;
    case Alignment::End:
      return start_point + content_length - child_length;
    default:
      return start_point;
  }
}
}  // namespace cru::ui::render
