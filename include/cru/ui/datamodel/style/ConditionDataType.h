#pragma once

#include "../../style/Condition.h"
#include "../Base.h"

namespace cru::ui::datamodel::style {
using cru::ui::style::Condition;

struct CRU_UI_API IConditionDataType : virtual Interface {
  virtual bool XmlElementIsOfThisType(xml::XmlElementNode* node) = 0;
  virtual DataConvertResult<ClonePtr<Condition>> ConvertConditionFromXml(
      xml::XmlElementNode* node) = 0;
};

#define CRU_DECLARE_CONDITION_DATA_TYPE(condition_name)                      \
  using ui::style::condition_name##Condition;                                \
  class CRU_UI_API condition_name##ConditionDataType                         \
      : public ClonePtrDataTypeBase<condition_name##Condition>,                \
        public virtual IConditionDataType {                                  \
   public:                                                                   \
    condition_name##ConditionDataType();                                     \
                                                                             \
    bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;         \
    DataConvertResult<ClonePtr<Condition>> ConvertConditionFromXml(          \
        xml::XmlElementNode* node) override;                                 \
                                                                             \
   protected:                                                                \
    bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;              \
    DataConvertResult<ClonePtr<condition_name##Condition>> DoConvertFromXml( \
        xml::XmlElementNode* node) override;                                 \
  };

CRU_DECLARE_CONDITION_DATA_TYPE(No)
CRU_DECLARE_CONDITION_DATA_TYPE(And)
CRU_DECLARE_CONDITION_DATA_TYPE(Or)
CRU_DECLARE_CONDITION_DATA_TYPE(ClickState)
CRU_DECLARE_CONDITION_DATA_TYPE(Checked)
CRU_DECLARE_CONDITION_DATA_TYPE(Focus)
CRU_DECLARE_CONDITION_DATA_TYPE(Hover)

#undef CRU_DECLARE_CONDITION_DATA_TYPE
}  // namespace cru::ui::datamodel::style
