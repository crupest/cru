#pragma once
#include "../Mapper.h"
#include <cru/CopyPtr.h>
#include "cru/ui/mapper/style/IStylerMapper.h"
#include "cru/ui/style/Styler.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
class CRU_UI_API BorderStylerMapper
    : public BasicCopyPtrMapper<ui::style::BorderStyler>,
      public virtual IStylerMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BorderStylerMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  CopyPtr<ui::style::Styler> MapStylerFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::BorderStyler> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
