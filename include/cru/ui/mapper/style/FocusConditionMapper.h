#pragma once
#include "../Mapper.h"
#include "IConditionMapper.h"
#include "cru/ui/style/Condition.h"

namespace cru::ui::mapper::style {
class CRU_UI_API FocusConditionMapper
    : public BasicCopyPtrMapper<ui::style::FocusCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(FocusConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  CopyPtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::FocusCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
