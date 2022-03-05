#pragma once
#include "../Mapper.h"
#include "IConditionMapper.h"
#include "cru/ui/style/Condition.h"

namespace cru::ui::mapper::style {
class CRU_UI_API HoverConditionMapper
    : public BasicClonablePtrMapper<ui::style::HoverCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(HoverConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::HoverCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
