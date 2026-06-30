#pragma once

#include "Base.h"

namespace cru::ui::datamodel {
class CRU_UI_API ThicknessDataType : public DataTypeBase<Thickness> {
 public:
  ThicknessDataType();

 protected:
  DataConvertResult<Thickness> DoConvertFromString(
      std::string_view value) override;
};
}  // namespace cru::ui::datamodel
