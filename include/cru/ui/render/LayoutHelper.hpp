#pragma once
#include "Base.hpp"

#include "MeasureRequirement.hpp"

namespace cru::ui::render {
float CalculateAnchorByAlignment(Alignment alignment, float start_point,
                                 float content_length, float child_length);

MeasureLength StackLayoutCalculateChildMaxLength(
    MeasureLength parent_preferred_size, MeasureLength parent_max_size,
    MeasureLength child_min_size, std::string_view exceeds_message);
}  // namespace cru::ui::render
