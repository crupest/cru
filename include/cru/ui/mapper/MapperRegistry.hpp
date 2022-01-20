#pragma once

#include "Mapper.hpp"

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

  void RegisterMapper(MapperBase* mapper);
  void UnregisterMapper(MapperBase* mapper);

 private:
  std::vector<MapperBase*> mapper_list_;
};
}  // namespace cru::ui::mapper
