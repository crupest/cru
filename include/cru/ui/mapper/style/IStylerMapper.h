#pragma once
#include "../../Base.h"
#include "cru/common/ClonablePtr.h"
#include "cru/ui/mapper/Mapper.h"
#include "cru/ui/style/Styler.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
struct CRU_UI_API IStylerMapper : virtual Interface {
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) {
    return dynamic_cast<MapperBase*>(this)->XmlElementIsOfThisType(node);
  }

  virtual ClonablePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) = 0;
};
}  // namespace cru::ui::mapper::style
