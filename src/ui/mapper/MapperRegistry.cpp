#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/mapper/BorderStyleMapper.hpp"
#include "cru/ui/mapper/ColorMapper.hpp"
#include "cru/ui/mapper/CornerRadiusMapper.hpp"
#include "cru/ui/mapper/CursorMapper.hpp"
#include "cru/ui/mapper/PointMapper.hpp"
#include "cru/ui/mapper/SizeMapper.hpp"
#include "cru/ui/mapper/ThicknessMapper.hpp"
#include "cru/ui/mapper/style/AndConditionMapper.hpp"
#include "cru/ui/mapper/style/BorderStylerMapper.hpp"
#include "cru/ui/mapper/style/ClickStateConditionMapper.hpp"
#include "cru/ui/mapper/style/CursorStylerMapper.hpp"
#include "cru/ui/mapper/style/FocusConditionMapper.hpp"
#include "cru/ui/mapper/style/HoverConditionMapper.hpp"
#include "cru/ui/mapper/style/NoConditionMapper.hpp"
#include "cru/ui/mapper/style/OrConditionMapper.hpp"
#include "cru/ui/mapper/style/StyleRuleMapper.hpp"
#include "cru/ui/mapper/style/StyleRuleSetMapper.hpp"

namespace cru::ui::mapper {
MapperRegistry *MapperRegistry::GetInstance() {
  static MapperRegistry instance;
  return &instance;
}

MapperRegistry::MapperRegistry() {
  using namespace style;

  RegisterMapper(new CornerRadiusMapper());
  RegisterMapper(new PointMapper());
  RegisterMapper(new SizeMapper());
  RegisterMapper(new ThicknessMapper());
  RegisterMapper(new BorderStyleMapper());
  RegisterMapper(new ColorMapper());
  RegisterMapper(new CursorMapper());
  RegisterMapper(new AndConditionMapper());
  RegisterMapper(new BorderStylerMapper());
  RegisterMapper(new ClickStateConditionMapper());
  RegisterMapper(new CursorStylerMapper());
  RegisterMapper(new FocusConditionMapper());
  RegisterMapper(new HoverConditionMapper());
  RegisterMapper(new NoConditionMapper());
  RegisterMapper(new OrConditionMapper());
  RegisterMapper(new StyleRuleMapper());
  RegisterMapper(new StyleRuleSetMapper());
}

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
