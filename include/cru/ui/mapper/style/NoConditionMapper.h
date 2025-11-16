#pragma once
#include "../Mapper.h"
#include "IConditionMapper.h"
#include "cru/base/Base.h"
#include "cru/base/ClonePtr.h"
#include "cru/ui/style/Condition.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper::style {
class CRU_UI_API NoConditionMapper
    : public BasicClonePtrMapper<ui::style::NoCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(NoConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonePtr<ui::style::NoCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
