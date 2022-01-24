#pragma once
#include "../Mapper.hpp"
#include "IConditionMapper.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API OrConditionMapper
    : public BasicPtrMapper<ui::style::OrCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(OrConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::OrCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
