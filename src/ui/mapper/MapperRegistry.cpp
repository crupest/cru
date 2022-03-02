#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/mapper/BorderStyleMapper.h"
#include "cru/ui/mapper/BrushMapper.h"
#include "cru/ui/mapper/ColorMapper.h"
#include "cru/ui/mapper/CornerRadiusMapper.h"
#include "cru/ui/mapper/CursorMapper.h"
#include "cru/ui/mapper/FontMapper.h"
#include "cru/ui/mapper/MeasureLengthMapper.h"
#include "cru/ui/mapper/PointMapper.h"
#include "cru/ui/mapper/SizeMapper.h"
#include "cru/ui/mapper/StringMapper.h"
#include "cru/ui/mapper/ThicknessMapper.h"
#include "cru/ui/mapper/style/AndConditionMapper.h"
#include "cru/ui/mapper/style/BorderStylerMapper.h"
#include "cru/ui/mapper/style/CheckedConditionMapper.h"
#include "cru/ui/mapper/style/ClickStateConditionMapper.h"
#include "cru/ui/mapper/style/CursorStylerMapper.h"
#include "cru/ui/mapper/style/FocusConditionMapper.h"
#include "cru/ui/mapper/style/HoverConditionMapper.h"
#include "cru/ui/mapper/style/MarginStylerMapper.h"
#include "cru/ui/mapper/style/NoConditionMapper.h"
#include "cru/ui/mapper/style/OrConditionMapper.h"
#include "cru/ui/mapper/style/PaddingStylerMapper.h"
#include "cru/ui/mapper/style/PreferredSizeStylerMapper.h"
#include "cru/ui/mapper/style/StyleRuleMapper.h"
#include "cru/ui/mapper/style/StyleRuleSetMapper.h"

namespace cru::ui::mapper {
MapperRegistry *MapperRegistry::GetInstance() {
  static MapperRegistry instance;
  return &instance;
}

MapperRegistry::MapperRegistry() {
  using namespace style;

  RegisterMapper(new BrushMapper());
  RegisterMapper(new CornerRadiusMapper());
  RegisterMapper(new FontMapper());
  RegisterMapper(new MeasureLengthMapper());
  RegisterMapper(new PointMapper());
  RegisterMapper(new SizeMapper());
  RegisterMapper(new StringMapper());
  RegisterMapper(new ThicknessMapper());
  RegisterMapper(new BorderStyleMapper());
  RegisterMapper(new ColorMapper());
  RegisterMapper(new CursorMapper());
  RegisterMapper(new AndConditionMapper());
  RegisterMapper(new BorderStylerMapper());
  RegisterMapper(new CheckedConditionMapper());
  RegisterMapper(new ClickStateConditionMapper());
  RegisterMapper(new CursorStylerMapper());
  RegisterMapper(new FocusConditionMapper());
  RegisterMapper(new HoverConditionMapper());
  RegisterMapper(new MarginStylerMapper());
  RegisterMapper(new NoConditionMapper());
  RegisterMapper(new OrConditionMapper());
  RegisterMapper(new PaddingStylerMapper());
  RegisterMapper(new PreferredSizeStylerMapper());
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
