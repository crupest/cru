#pragma once
#include "Mapper.h"

namespace cru::ui::mapper {
class CRU_UI_API SizeMapper : public BasicMapper<Size> {
  CRU_UI_DECLARE_CAN_MAP_FROM_STRING(Size)
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(Size, Size)
};
}  // namespace cru::ui::mapper
