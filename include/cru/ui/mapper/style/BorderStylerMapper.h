#pragma once
#include "../Mapper.h"
#include "cru/base/ClonablePtr.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
class CRU_UI_API BorderStylerMapper
    : public BasicClonablePtrMapper<ui::style::BorderStyler>,
      public virtual IStylerMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BorderStylerMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::BorderStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
