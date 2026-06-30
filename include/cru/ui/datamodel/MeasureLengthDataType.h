#pragma once

#include "../render/MeasureRequirement.h"
#include "Base.h"

namespace cru::ui::datamodel {
class CRU_UI_API MeasureLengthDataType
    : public DataTypeBase<render::MeasureLength> {
 public:
  MeasureLengthDataType();

 protected:
  DataConvertResult<render::MeasureLength> DoConvertFromString(
      std::string_view value) override;
};
}  // namespace cru::ui::datamodel
