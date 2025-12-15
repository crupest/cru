#pragma once
#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API ThicknessMapper : public BasicMapper<Thickness> {
  CRU_UI_DECLARE_CAN_MAP_FROM_STRING(Thickness)
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(Thickness, Thickness)
};
}  // namespace cru::ui::mapper
