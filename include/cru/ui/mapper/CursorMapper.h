#pragma once
#include "Mapper.h"

#include <cru/platform/gui/Cursor.h>

namespace cru::ui::mapper {
class CRU_UI_API CursorMapper
    : public BasicSharedPtrMapper<platform::gui::ICursor> {
  CRU_UI_DECLARE_CAN_MAP_FROM_STRING(std::shared_ptr<platform::gui::ICursor>)
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(
      Cursor, std::shared_ptr<platform::gui::ICursor>)
};
}  // namespace cru::ui::mapper
