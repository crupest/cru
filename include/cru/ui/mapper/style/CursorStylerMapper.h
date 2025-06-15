#pragma once
#include "../Mapper.h"
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
class CRU_UI_API CursorStylerMapper
    : public BasicCopyPtrMapper<ui::style::CursorStyler>,
      public virtual IStylerMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(CursorStylerMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  CopyPtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::CursorStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
