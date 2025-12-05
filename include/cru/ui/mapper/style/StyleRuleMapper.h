#pragma once
#include "../Mapper.h"
#include "cru/base/Base.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/style/StyleRule.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper::style {
class CRU_UI_API StyleRuleMapper : public BasicClonePtrMapper<ui::style::StyleRule> {
 private:
  constexpr static auto kLogTag = "StyleRuleMapper";

 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(StyleRuleMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  ClonePtr<ui::style::StyleRule> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
