#pragma once
#include "../Mapper.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper::style {
class CRU_UI_API BorderStylerMapper
    : public BasicClonePtrMapper<ui::style::BorderStyler>,
      public virtual IStylerMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BorderStylerMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonePtr<ui::style::BorderStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
