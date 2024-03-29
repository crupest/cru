#pragma once
#include <memory>
#include "../../style/StyleRuleSet.h"
#include "../Mapper.h"

namespace cru::ui::mapper::style {
class CRU_UI_API StyleRuleSetMapper
    : public BasicSharedPtrMapper<ui::style::StyleRuleSet> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(StyleRuleSetMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::shared_ptr<ui::style::StyleRuleSet> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
;
}  // namespace cru::ui::mapper::style
