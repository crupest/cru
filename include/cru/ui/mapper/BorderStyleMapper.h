#pragma once
#include "Mapper.h"
#include "cru/ui/style/ApplyBorderStyleInfo.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper {
class CRU_UI_API BorderStyleMapper
    : public BasicMapper<ui::style::ApplyBorderStyleInfo> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BorderStyleMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  ui::style::ApplyBorderStyleInfo DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
