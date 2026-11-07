#include "cru/base/datamodel/DataType.h"
#include "cru/base/datamodel/DataTypeRegistry.h"

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

ObjectDataType::ObjectDataType(DataTypeRegistry* registry)
    : DataTypeBase<KeyValueObject>("Object"), registry_(registry) {}

void ObjectDataType::ConvertToStringImpl(const KeyValueObject& value,
                                         DataConvertResult<std::string>& result,
                                         std::string& result_value,
                                         std::string& path) {
  result_value += "{ ";
  bool first = true;
  for (const auto& [key, val] : value) {
    path += key;

    if (!first) {
      result_value += ", ";
    } else {
      first = false;
    }
    result_value += std::format("{} : ", key);

    if (val.has_value()) {
      if (val.type() == typeid(KeyValueObject)) {
        path += ".";
        ConvertToStringImpl(std::any_cast<const KeyValueObject&>(val), result,
                            result_value, path);
        path.erase(path.size() - 1);
      } else {
        auto data_type = registry_->GetDataTypeOfValue(val);
        if (data_type) {
          auto convert_result = data_type->ConvertToString(val);
          if (convert_result.IsSuccess()) {
            result_value += convert_result.GetValue();
          } else {
            result_value += "CONVERSION-ERROR";
          }
          for (const auto& error : convert_result.GetErrors()) {
            result.AddError(std::format("In property {}, {}", path, error));
          }
        } else {
          result_value += "UNKNOWN-TYPE";
        }
      }
    } else {
      result_value += "EMPTY-ANY";
    }

    path.erase(path.size() - key.size());
  }
  result_value += " }";
}

}  // namespace cru::datamodel
