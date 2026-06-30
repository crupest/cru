#include "cru/ui/datamodel/Base.h"
#include "cru/base/datamodel/DataTypeRegistry.h"
#include "cru/ui/datamodel/BorderStyleDataType.h"
#include "cru/ui/datamodel/BrushDataType.h"
#include "cru/ui/datamodel/ColorDataType.h"
#include "cru/ui/datamodel/CornerRadiusDataType.h"
#include "cru/ui/datamodel/CursorDataType.h"
#include "cru/ui/datamodel/FontDataType.h"
#include "cru/ui/datamodel/MeasureLengthDataType.h"
#include "cru/ui/datamodel/PointDataType.h"
#include "cru/ui/datamodel/SizeDataType.h"
#include "cru/ui/datamodel/ThicknessDataType.h"
#include "cru/ui/datamodel/style/ConditionDataType.h"
#include "cru/ui/datamodel/style/StyleRuleDataType.h"
#include "cru/ui/datamodel/style/StyleRuleSetDataType.h"
#include "cru/ui/datamodel/style/StylerDataType.h"

namespace cru::ui::datamodel {

static cru::datamodel::DataTypeRegistry* CreateUiDataTypeRegistry() {
  auto registry = new cru::datamodel::DataTypeRegistry();

  using namespace style;

  registry->RegisterDataType(new BrushDataType());
  registry->RegisterDataType(new CornerRadiusDataType());
  registry->RegisterDataType(new FontDataType());
  registry->RegisterDataType(new MeasureLengthDataType());
  registry->RegisterDataType(new PointDataType());
  registry->RegisterDataType(new SizeDataType());
  registry->RegisterDataType(new StringDataType());
  registry->RegisterDataType(new ThicknessDataType());
  registry->RegisterDataType(new BorderStyleDataType());
  registry->RegisterDataType(new ColorDataType());
  registry->RegisterDataType(new CursorDataType());
  registry->RegisterDataType(new AndConditionDataType());
  registry->RegisterDataType(new BorderStylerDataType());
  registry->RegisterDataType(new CheckedConditionDataType());
  registry->RegisterDataType(new ClickStateConditionDataType());
  registry->RegisterDataType(new ContentBrushStylerDataType());
  registry->RegisterDataType(new CursorStylerDataType());
  registry->RegisterDataType(new FocusConditionDataType());
  registry->RegisterDataType(new FontStylerDataType());
  registry->RegisterDataType(new HoverConditionDataType());
  registry->RegisterDataType(new MarginStylerDataType());
  registry->RegisterDataType(new NoConditionDataType());
  registry->RegisterDataType(new OrConditionDataType());
  registry->RegisterDataType(new PaddingStylerDataType());
  registry->RegisterDataType(new PreferredSizeStylerDataType());
  registry->RegisterDataType(new StyleRuleDataType());
  registry->RegisterDataType(new StyleRuleSetDataType());
  return registry;
}

cru::datamodel::DataTypeRegistry* GetUiDataTypeRegistry() {
  static cru::datamodel::DataTypeRegistry* registry =
      CreateUiDataTypeRegistry();
  return registry;
}
}  // namespace cru::ui::datamodel
