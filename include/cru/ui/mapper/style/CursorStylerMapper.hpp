#pragma once
#include "../Mapper.hpp"
#include "cru/ui/mapper/style/IStylerMapper.hpp"
#include "cru/ui/style/Styler.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API CursorStylerMapper
    : public BasicPtrMapper<ui::style::CursorStyler>,
      public virtual IStylerMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(CursorStylerMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::CursorStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
