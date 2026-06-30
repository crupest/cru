#include "cru/ui/datamodel/MeasureLengthDataType.h"

#include "cru/base/StringUtil.h"

namespace cru::ui::datamodel {
MeasureLengthDataType::MeasureLengthDataType()
    : DataTypeBase<render::MeasureLength>("MeasureLength",
                                            {true, false, false, false}) {}

DataConvertResult<render::MeasureLength>
MeasureLengthDataType::DoConvertFromString(std::string_view value) {
  if (value.empty()) {
    return DataConvertResult<render::MeasureLength>::Success(
        render::MeasureLength::NotSpecified());
  }

  if (cru::string::CaseInsensitiveCompare(value, "notspecified") == 0) {
    return DataConvertResult<render::MeasureLength>::Success(
        render::MeasureLength::NotSpecified());
  }
  if (cru::string::CaseInsensitiveCompare(value, "unspecified") == 0) {
    return DataConvertResult<render::MeasureLength>::Success(
        render::MeasureLength::NotSpecified());
  }

  auto parse_result = cru::string::ParseToNumber<float>(value);
  // NOTE: Legacy mapper used ParseToNumber(...).value without checking valid.
  if (!parse_result.valid) {
    return DataConvertResult<render::MeasureLength>::Failure(
        "Invalid MeasureLength string.");
  }

  if (parse_result.value < 0) {
    return DataConvertResult<render::MeasureLength>::Success(
        render::MeasureLength::NotSpecified());
  }

  return DataConvertResult<render::MeasureLength>::Success(
      render::MeasureLength(parse_result.value));
}
}  // namespace cru::ui::datamodel
