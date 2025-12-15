#pragma once
#include "../render/MeasureRequirement.h"
#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API MeasureLengthMapper
    : public BasicMapper<render::MeasureLength> {
  CRU_UI_DECLARE_CAN_MAP_FROM_STRING(render::MeasureLength)
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(MeasureLength,
                                              render::MeasureLength)
};
}  // namespace cru::ui::mapper
