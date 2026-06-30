#pragma once

#include "Base.h"

#include <cru/platform/gui/Cursor.h>

namespace cru::ui::datamodel {
class CRU_UI_API CursorDataType
    : public SharedPtrDataTypeBase<platform::gui::ICursor> {
 public:
  CursorDataType();

 protected:
  DataConvertResult<std::shared_ptr<platform::gui::ICursor>>
  DoConvertFromString(std::string_view value) override;
};
}  // namespace cru::ui::datamodel
