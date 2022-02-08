#pragma once
#include "Base.h"

#include "MeasureRequirement.h"

namespace cru::ui::render {
float CalculateAnchorByAlignment(Alignment alignment, float start_point,
                                 float content_length, float child_length);

MeasureLength StackLayoutCalculateChildMaxLength(
    MeasureLength parent_preferred_size, MeasureLength parent_max_size,
    MeasureLength child_min_size, std::u16string_view log_tag,
    std::u16string_view exceeds_message);
}  // namespace cru::ui::render
