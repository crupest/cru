#pragma once

#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API CornerRadiusMapper : public BasicMapper<CornerRadius> {
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(CornerRadius, CornerRadius)
};
}  // namespace cru::ui::mapper
