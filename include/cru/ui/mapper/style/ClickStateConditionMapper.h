#pragma once
#include "../Mapper.h"
#include "IConditionMapper.h"
#include "cru/ui/style/Condition.h"

namespace cru::ui::mapper::style {
class CRU_UI_API ClickStateConditionMapper
    : public BasicCopyPtrMapper<ui::style::ClickStateCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ClickStateConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  CopyPtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 public:
  CopyPtr<ui::style::ClickStateCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
