#pragma once
#include "../Mapper.hpp"
#include "IConditionMapper.hpp"
#include "cru/ui/style/Condition.hpp"

namespace cru::ui::mapper::style {
class FocusConditionMapper : public BasicPtrMapper<ui::style::FocusCondition>,
                             public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(FocusConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::FocusCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
