#include "cru/ui/datamodel/PointDataType.h"

#include "cru/base/StringUtil.h"

namespace cru::ui::datamodel {
PointDataType::PointDataType()
    : DataTypeBase<Point>("Point", {true, false, false, false}) {}

DataConvertResult<Point> PointDataType::DoConvertFromString(
    std::string_view value) {
  if (value.empty()) {
    return DataConvertResult<Point>::Success(Point{});
  }

  std::vector<float> values = cru::string::ParseToNumberList<float>(value);
  if (values.size() == 2) {
    return DataConvertResult<Point>::Success({values[0], values[1]});
  }
  if (values.size() == 1) {
    return DataConvertResult<Point>::Success({values[0], values[0]});
  }
  return DataConvertResult<Point>::Failure("Invalid Point string.");
}
}  // namespace cru::ui::datamodel
