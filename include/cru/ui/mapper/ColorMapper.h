#pragma once
#include "Mapper.h"

#include <cru/base/xml/XmlNode.h>
#include <cru/platform/Color.h>

namespace cru::ui::mapper {
class CRU_UI_API ColorMapper : public BasicMapper<Color> {
  CRU_UI_DECLARE_CAN_MAP_FROM_STRING(Color)
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(Color, Color)
};
}  // namespace cru::ui::mapper
