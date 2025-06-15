#pragma once
#include "../Mapper.h"
#include <cru/Base.h>
#include <cru/CopyPtr.h>
#include "cru/ui/style/StyleRule.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper::style {
class CRU_UI_API StyleRuleMapper : public BasicCopyPtrMapper<ui::style::StyleRule> {
  CRU_DEFINE_CLASS_LOG_TAG(u"StyleRuleMapper")
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(StyleRuleMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  CopyPtr<ui::style::StyleRule> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
