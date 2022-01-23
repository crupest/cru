#pragma once
#include "../Mapper.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/style/Condition.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper::style {
class NoConditionMapper : public BasicPtrMapper<ui::style::NoCondition> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(NoConditionMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  ClonablePtr<ui::style::NoCondition> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper::style
