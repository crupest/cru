#pragma once
#include "../Mapper.hpp"
#include "IConditionMapper.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/style/Condition.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
class CRU_UI_API NoConditionMapper
    : public BasicPtrMapper<ui::style::NoCondition>,
      public virtual IConditionMapper {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(NoConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

  ClonablePtr<ui::style::Condition> MapConditionFromXml(
      xml::XmlElementNode* node) override {
    return MapFromXml(node);
  }

 protected:
  ClonablePtr<ui::style::NoCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
