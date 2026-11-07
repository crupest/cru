#pragma once

#include "../Base.h"
#include "../ClonePtr.h"
#include "DataType.h"

#include <vector>

namespace cru::datamodel {
class CRU_BASE_API DataTypeRegistry : public Object {
 public:
  DataTypeRegistry();
  ~DataTypeRegistry();

  const std::vector<IDataTypeBase*>& GetAllDataTypes() {
    return data_type_list_;
  }

  IDataTypeBase* GetDataType(std::string_view name) {
    for (auto* data_type : data_type_list_) {
      if (data_type->GetName() == name) {
        return data_type;
      }
    }
    return nullptr;
  }

  IDataTypeBase* GetDataType(const std::type_info& type_info) {
    for (auto* data_type : data_type_list_) {
      if (data_type->GetTypeInfo() == type_info) {
        return data_type;
      }
    }
    return nullptr;
  }

  template <typename T, typename DT = DataTypeBase<T>>
  DT* GetDataType()
    requires(std::is_base_of_v<DataTypeBase<T>, DT>)
  {
    for (auto* data_type : data_type_list_) {
      auto* typed = dynamic_cast<DT*>(data_type);
      if (typed) {
        return typed;
      }
    }
    return nullptr;
  }

  IDataTypeBase* GetDataTypeOfValue(const std::any& value) {
    if (!value.has_value()) {
      return nullptr;
    }
    return GetDataType(value.type());
  }

  template <typename T>
  DataTypeBase<std::shared_ptr<T>>* GetSharedPtrDataType() {
    return GetDataType<std::shared_ptr<T>>();
  }

  template <typename T>
  DataTypeBase<ClonePtr<T>>* GetClonePtrDataType() {
    return GetDataType<ClonePtr<T>>();
  }

  template <typename T>
  std::vector<T*> GetDataTypesByInterface() {
    std::vector<T*> result;
    for (auto* data_type : data_type_list_) {
      auto* typed = dynamic_cast<T*>(data_type);
      if (typed) {
        result.push_back(typed);
      }
    }
    return result;
  }

  void RegisterDataType(IDataTypeBase* data_type);
  void UnregisterDataType(IDataTypeBase* data_type);

 private:
  std::vector<IDataTypeBase*> data_type_list_;
};
}  // namespace cru::datamodel
