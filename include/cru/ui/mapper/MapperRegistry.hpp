#pragma once

#include "Mapper.hpp"

#include <memory>
#include <vector>

namespace cru::ui::mapper {
class CRU_UI_API MapperRegistry {
 public:
  MapperRegistry();

  CRU_DELETE_COPY(MapperRegistry)
  CRU_DELETE_MOVE(MapperRegistry)

  ~MapperRegistry();

  const std::vector<MapperBase*>& GetAllMappers() const { return mapper_list_; }

  void RegisterMapper(MapperBase* mapper);
  void UnregisterMapper(MapperBase* mapper);

 private:
  std::vector<MapperBase*> mapper_list_;
};
}  // namespace cru::ui::mapper
