#pragma once
#include "../Mapper.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
class BorderStyleMapper : BasicMapper<ui::style::ApplyBorderStyleInfo> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BorderStyleMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::unique_ptr<ui::style::ApplyBorderStyleInfo> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
