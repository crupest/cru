#include "cru/ui/datamodel/ThicknessDataType.h"

#include "cru/base/StringUtil.h"

namespace cru::ui::datamodel {
ThicknessDataType::ThicknessDataType()
    : DataTypeBase<Thickness>("Thickness", {true, false, false, false}) {}

DataConvertResult<Thickness> ThicknessDataType::DoConvertFromString(
    std::string_view value) {
  if (value.empty()) {
    return DataConvertResult<Thickness>::Success(Thickness{});
  }

  std::vector<float> values = cru::string::ParseToNumberList<float>(value);
  if (values.size() == 4) {
    return DataConvertResult<Thickness>::Success(
        Thickness(values[0], values[1], values[2], values[3]));
  }
  if (values.size() == 2) {
    return DataConvertResult<Thickness>::Success(
        Thickness(values[0], values[1], values[0], values[1]));
  }
  if (values.size() == 1) {
    return DataConvertResult<Thickness>::Success(
        Thickness(values[0], values[0], values[0], values[0]));
  }
  return DataConvertResult<Thickness>::Failure("Invalid Thickness string.");
}
}  // namespace cru::ui::datamodel
