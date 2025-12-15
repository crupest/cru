#pragma once
#include "Mapper.h"

#include <cru/platform/graphics/Font.h>

namespace cru::ui::mapper {
class FontMapper : public BasicSharedPtrMapper<platform::graphics::IFont> {
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(
      Font, std::shared_ptr<platform::graphics::IFont>)
};
}  // namespace cru::ui::mapper
