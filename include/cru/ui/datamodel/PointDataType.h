#pragma once

#include "Base.h"

namespace cru::ui::datamodel {
class CRU_UI_API PointDataType : public DataTypeBase<Point> {
 public:
  PointDataType();

 protected:
  DataConvertResult<Point> DoConvertFromString(std::string_view value) override;
};
}  // namespace cru::ui::datamodel
