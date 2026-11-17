#include "cru/base/datamodel/DataTypeRegistry.h"
#include "cru/base/datamodel/DataType.h"

namespace cru::datamodel {
std::unique_ptr<DataTypeRegistry> DataTypeRegistry::CreateDefaultRegistry() {
  auto registry = std::make_unique<DataTypeRegistry>();

  registry->RegisterDataType(new CharDataType());
  registry->RegisterDataType(new SignedCharDataType());
  registry->RegisterDataType(new UnsignedCharDataType());
  registry->RegisterDataType(new ShortDataType());
  registry->RegisterDataType(new UnsignedShortDataType());
  registry->RegisterDataType(new IntDataType());
  registry->RegisterDataType(new UnsignedIntDataType());
  registry->RegisterDataType(new LongDataType());
  registry->RegisterDataType(new UnsignedLongDataType());
  registry->RegisterDataType(new LongLongDataType());
  registry->RegisterDataType(new UnsignedLongLongDataType());
  registry->RegisterDataType(new FloatDataType());
  registry->RegisterDataType(new DoubleDataType());
  registry->RegisterDataType(new LongDoubleDataType());

  registry->RegisterDataType(new CharListDataType());
  registry->RegisterDataType(new SignedCharListDataType());
  registry->RegisterDataType(new UnsignedCharListDataType());
  registry->RegisterDataType(new ShortListDataType());
  registry->RegisterDataType(new UnsignedShortListDataType());
  registry->RegisterDataType(new IntListDataType());
  registry->RegisterDataType(new UnsignedIntListDataType());
  registry->RegisterDataType(new LongListDataType());
  registry->RegisterDataType(new UnsignedLongListDataType());
  registry->RegisterDataType(new LongLongListDataType());
  registry->RegisterDataType(new UnsignedLongLongListDataType());
  registry->RegisterDataType(new FloatListDataType());
  registry->RegisterDataType(new DoubleListDataType());
  registry->RegisterDataType(new LongDoubleListDataType());

  registry->RegisterDataType(new StringDataType());
  registry->RegisterDataType(new ObjectDataType(registry.get()));

  return registry;
}

DataTypeRegistry::DataTypeRegistry() {}

DataTypeRegistry::~DataTypeRegistry() {
  for (auto* data_type : data_type_list_) {
    delete data_type;
  }
}

void DataTypeRegistry::RegisterDataType(IDataTypeBase* data_type) {
  if (std::find(data_type_list_.cbegin(), data_type_list_.cend(), data_type) !=
      data_type_list_.cend()) {
    throw Exception("This data type is already registered.");
  }

  data_type_list_.push_back(data_type);
}

void DataTypeRegistry::UnregisterDataType(IDataTypeBase* data_type) {
  auto it =
      std::find(data_type_list_.begin(), data_type_list_.end(), data_type);
  if (it == data_type_list_.end()) {
    throw Exception("This data type is not registered.");
  }

  data_type_list_.erase(it);
}
}  // namespace cru::datamodel
