#pragma once
#include "../../Base.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/mapper/Mapper.hpp"
#include "cru/ui/style/Styler.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
struct CRU_UI_API IStylerMapper : virtual Interface {
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) {
    return dynamic_cast<MapperBase*>(this)->XmlElementIsOfThisType(node);
  }

  virtual ClonablePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) = 0;
};
}  // namespace cru::ui::mapper::style
