#pragma once

#include "../../style/Styler.h"
#include "../Base.h"

namespace cru::ui::datamodel::style {
using cru::ui::style::ApplyBorderStyleInfo;
using cru::ui::style::Styler;

struct CRU_UI_API IStylerDataType : virtual Interface {
  virtual bool XmlElementIsOfThisType(xml::XmlElementNode* node) = 0;
  virtual DataConvertResult<ClonePtr<Styler>> ConvertStylerFromXml(
      xml::XmlElementNode* node) = 0;
};

#define CRU_DECLARE_STYLER_DATA_TYPE(styler_name)                      \
  using ui::style::styler_name##Styler;                                \
  class CRU_UI_API styler_name##StylerDataType                         \
      : public ClonePtrDataTypeBase<styler_name##Styler>,                \
        public virtual IStylerDataType {                               \
   public:                                                             \
    styler_name##StylerDataType();                                     \
                                                                       \
    bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;   \
    DataConvertResult<ClonePtr<Styler>> ConvertStylerFromXml(          \
        xml::XmlElementNode* node) override;                           \
                                                                       \
   protected:                                                          \
    bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;        \
    DataConvertResult<ClonePtr<styler_name##Styler>> DoConvertFromXml( \
        xml::XmlElementNode* node) override;                           \
  };

CRU_DECLARE_STYLER_DATA_TYPE(Border)
CRU_DECLARE_STYLER_DATA_TYPE(ContentBrush)
CRU_DECLARE_STYLER_DATA_TYPE(Cursor)
CRU_DECLARE_STYLER_DATA_TYPE(Font)
CRU_DECLARE_STYLER_DATA_TYPE(Margin)
CRU_DECLARE_STYLER_DATA_TYPE(Padding)
CRU_DECLARE_STYLER_DATA_TYPE(PreferredSize)

#undef CRU_DECLARE_STYLER_DATA_TYPE
}  // namespace cru::ui::datamodel::style
