#pragma once
#include "MeasureRequirement.h"

namespace cru::ui::render {
float CalculateAnchorByAlignment(Alignment alignment, float start_point,
                                 float content_length, float child_length);

}  // namespace cru::ui::render
