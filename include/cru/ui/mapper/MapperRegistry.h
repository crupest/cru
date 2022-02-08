#pragma once

#include "Mapper.h"

#include <memory>
#include <vector>

namespace cru::ui::mapper {
class CRU_UI_API MapperRegistry {
 public:
  static MapperRegistry* GetInstance();

  MapperRegistry();

  CRU_DELETE_COPY(MapperRegistry)
  CRU_DELETE_MOVE(MapperRegistry)

  ~MapperRegistry();

  const std::vector<MapperBase*>& GetAllMappers() const { return mapper_list_; }

  template <typename T>
  BasicMapper<T>* GetMapper() const {
    for (auto mapper : mapper_list_) {
      if (mapper->GetTypeIndex() == typeid(T)) {
        return static_cast<BasicMapper<T>*>(mapper);
      }
    }
    return nullptr;
  }

  template <typename T>
  BasicRefMapper<T>* GetRefMapper() const {
    return GetMapper<std::shared_ptr<T>>();
  }

  template <typename T>
  BasicPtrMapper<T>* GetPtrMapper() const {
    return GetMapper<ClonablePtr<T>>();
  }

  template <typename T>
  std::vector<T*> GetMappersByInterface() const {
    std::vector<T*> result;
    for (auto mapper : mapper_list_) {
      auto m = dynamic_cast<T*>(mapper);
      if (m) result.push_back(m);
    }
    return result;
  }

  void RegisterMapper(MapperBase* mapper);
  void UnregisterMapper(MapperBase* mapper);

 private:
  std::vector<MapperBase*> mapper_list_;
};
}  // namespace cru::ui::mapper
