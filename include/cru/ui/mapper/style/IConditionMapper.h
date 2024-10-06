#pragma once
#include "../../Base.h"
#include "cru/base/ClonablePtr.h"
#include "cru/ui/mapper/Mapper.h"
#include "cru/ui/style/Condition.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
struct CRU_UI_API IConditionMapper : virtual Interface {
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) {
    return dynamic_cast<MapperBase*>(this)->XmlElementIsOfThisType(node);
  }

  virtual ClonablePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) = 0;
};
}  // namespace cru::ui::mapper::style
