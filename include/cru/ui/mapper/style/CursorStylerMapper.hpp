#pragma once
#include "../Mapper.hpp"
#include "cru/ui/style/Styler.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API CursorStylerMapper
    : public BasicPtrMapper<ui::style::CursorStyler> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(CursorStylerMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  ClonablePtr<ui::style::CursorStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
