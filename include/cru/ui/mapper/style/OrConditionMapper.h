#pragma once
#include "../Mapper.h"
#include "IConditionMapper.h"

namespace cru::ui::mapper::style {
class CRU_UI_API OrConditionMapper
    : public BasicCopyPtrMapper<ui::style::OrCondition>,
      public virtual IConditionMapper {
 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  CopyPtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  CopyPtr<ui::style::OrCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
