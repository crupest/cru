#pragma once
#include "../Mapper.hpp"
#include "IConditionMapper.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API AndConditionMapper
    : public BasicPtrMapper<ui::style::AndCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(AndConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::AndCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
