#pragma once
#include "../Mapper.hpp"
#include "IConditionMapper.hpp"
#include "cru/ui/style/Condition.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API ClickStateConditionMapper
    : public BasicPtrMapper<ui::style::ClickStateCondition>,
      public IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ClickStateConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 public:
  ClonablePtr<ui::style::ClickStateCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
