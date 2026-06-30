#include "cru/ui/datamodel/SizeDataType.h"

#include "cru/base/StringUtil.h"

namespace cru::ui::datamodel {
SizeDataType::SizeDataType()
    : DataTypeBase<Size>("Size", {true, false, false, false}) {}

DataConvertResult<Size> SizeDataType::DoConvertFromString(
    std::string_view value) {
  if (value.empty()) {
    return DataConvertResult<Size>::Success(Size{});
  }

  std::vector<float> values = cru::string::ParseToNumberList<float>(value);
  if (values.size() == 2) {
    return DataConvertResult<Size>::Success({values[0], values[1]});
  }
  if (values.size() == 1) {
    return DataConvertResult<Size>::Success({values[0], values[0]});
  }
  return DataConvertResult<Size>::Failure("Invalid Size string.");
}
}  // namespace cru::ui::datamodel
