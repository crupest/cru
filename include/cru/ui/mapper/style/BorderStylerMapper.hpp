#pragma once
#include "../Mapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/mapper/style/IStylerMapper.hpp"
#include "cru/ui/style/Styler.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API BorderStylerMapper
    : public BasicPtrMapper<ui::style::BorderStyler>,
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
