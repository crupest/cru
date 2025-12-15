#pragma once
#include "Mapper.h"

#include <string>

namespace cru::ui::mapper {
class CRU_UI_API StringMapper : public BasicMapper<std::string> {
  CRU_UI_DECLARE_CAN_MAP_FROM_STRING(std::string)
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(String, std::string)
};
}  // namespace cru::ui::mapper
