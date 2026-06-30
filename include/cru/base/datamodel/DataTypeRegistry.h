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

  const std::vector<IDataType*>& GetAllDataTypes() { return data_type_list_; }

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

  void RegisterDataType(IDataType* data_type);
  void UnregisterDataType(IDataType* data_type);

 private:
  std::vector<IDataType*> data_type_list_;
};
}  // namespace cru::datamodel
