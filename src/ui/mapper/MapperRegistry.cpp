#include "cru/ui/mapper/MapperRegistry.hpp"

namespace cru::ui::mapper {
MapperRegistry::MapperRegistry() {}

MapperRegistry::~MapperRegistry() {
  for (auto mapper : mapper_list_) {
    delete mapper;
  }
}

void MapperRegistry::RegisterMapper(MapperBase *mapper) {
  if (std::find(mapper_list_.cbegin(), mapper_list_.cend(), mapper) !=
      mapper_list_.cend()) {
    throw Exception(u"This mapper is already registered.");
  }

  mapper_list_.push_back(mapper);
}

void MapperRegistry::UnregisterMapper(MapperBase *mapper) {
  auto it = std::find(mapper_list_.begin(), mapper_list_.end(), mapper);
  if (it == mapper_list_.end()) {
    throw Exception(u"This mapper is not registered.");
  }

  mapper_list_.erase(it);
}
}  // namespace cru::ui::mapper
