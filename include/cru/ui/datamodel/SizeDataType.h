#pragma once

#include "Base.h"

namespace cru::ui::datamodel {
class CRU_UI_API SizeDataType : public DataTypeBase<Size> {
 public:
  SizeDataType();

 protected:
  DataConvertResult<Size> DoConvertFromString(std::string_view value) override;
};
}  // namespace cru::ui::datamodel
