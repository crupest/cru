#include "cru/base/datamodel/DataType.h"

namespace cru::datamodel {
StringDataType::StringDataType() : DataTypeBase<std::string>("String") {}

DataConvertResult<std::string> StringDataType::DoConvertFromString(
    std::string_view value) {
  return DataConvertResult<std::string>::Success(std::string(value));
}
DataConvertResult<std::string> StringDataType::DoConvertToString(
    const std::string& value) {
  return DataConvertResult<std::string>::Success(value);
}
}  // namespace cru::datamodel
