#include "cru/base/datamodel/DataTypeRegistry.h"

namespace cru::datamodel {
DataTypeRegistry::DataTypeRegistry() {}

DataTypeRegistry::~DataTypeRegistry() {
  for (auto* data_type : data_type_list_) {
    delete data_type;
  }
}

void DataTypeRegistry::RegisterDataType(IDataType* data_type) {
  if (std::find(data_type_list_.cbegin(), data_type_list_.cend(), data_type) !=
      data_type_list_.cend()) {
    throw Exception("This data type is already registered.");
  }

  data_type_list_.push_back(data_type);
}

void DataTypeRegistry::UnregisterDataType(IDataType* data_type) {
  auto it =
      std::find(data_type_list_.begin(), data_type_list_.end(), data_type);
  if (it == data_type_list_.end()) {
    throw Exception("This data type is not registered.");
  }

  data_type_list_.erase(it);
}
}  // namespace cru::datamodel
