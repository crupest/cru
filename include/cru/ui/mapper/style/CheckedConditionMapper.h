#pragma once
#include "../Mapper.h"
#include "IConditionMapper.h"
#include "cru/ui/style/Condition.h"

namespace cru::ui::mapper::style {
class CRU_UI_API CheckedConditionMapper
    : public BasicCopyPtrMapper<ui::style::CheckedCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(CheckedConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  CopyPtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::CheckedCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
