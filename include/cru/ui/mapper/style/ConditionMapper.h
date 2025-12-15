#pragma once
#include "../../Base.h"
#include "../../style/Condition.h"
#include "../Mapper.h"

#include <cru/base/ClonePtr.h>
#include <cru/base/xml/XmlNode.h>

namespace cru::ui::mapper::style {
using ui::style::Condition;

struct CRU_UI_API IConditionMapper : virtual Interface {
  virtual bool XmlElementIsOfThisType(xml::XmlElementNode* node) = 0;
  virtual ClonePtr<Condition> MapConditionFromXml(
      xml::XmlElementNode* node) = 0;
};

#define CRU_DECLARE_CONDITION_MAPPER(condition_name)                    \
  using ui::style::condition_name##Condition;                           \
  class CRU_UI_API condition_name##ConditionMapper                      \
      : public BasicClonePtrMapper<condition_name##Condition>,          \
        public virtual IConditionMapper {                               \
    CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(                        \
        condition_name##Condition, ClonePtr<condition_name##Condition>) \
                                                                        \
    ClonePtr<Condition> MapConditionFromXml(                            \
        xml::XmlElementNode* node) override {                           \
      return MapFromXml(node);                                          \
    }                                                                   \
  };

CRU_DECLARE_CONDITION_MAPPER(No)
CRU_DECLARE_CONDITION_MAPPER(And)
CRU_DECLARE_CONDITION_MAPPER(Or)
CRU_DECLARE_CONDITION_MAPPER(ClickState)
CRU_DECLARE_CONDITION_MAPPER(Checked)
CRU_DECLARE_CONDITION_MAPPER(Focus)
CRU_DECLARE_CONDITION_MAPPER(Hover)

#undef CRU_DECLARE_CONDITION_MAPPER
}  // namespace cru::ui::mapper::style
