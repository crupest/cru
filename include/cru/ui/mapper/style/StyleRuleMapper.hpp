#pragma once
#include "../Mapper.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/style/StyleRule.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API StyleRuleMapper : public BasicPtrMapper<ui::style::StyleRule> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(StyleRuleMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  ClonablePtr<ui::style::StyleRule> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
