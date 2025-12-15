#pragma once
#include "../../Base.h"
#include "cru/base/ClonePtr.h"
#include "cru/base/xml/XmlNode.h"
#include "cru/ui/mapper/Mapper.h"
#include "cru/ui/style/Styler.h"

namespace cru::ui::mapper::style {
using ui::style::Styler;
using cru::ui::style::ApplyBorderStyleInfo;

struct CRU_UI_API IStylerMapper : virtual Interface {
  virtual bool XmlElementIsOfThisType(xml::XmlElementNode* node) = 0;
  virtual ClonePtr<Styler> MapStylerFromXml(xml::XmlElementNode* node) = 0;
};

#define CRU_DECLARE_STYLER_MAPPER(styler_name)                                 \
  using ui::style::styler_name##Styler;                                        \
  class CRU_UI_API styler_name##StylerMapper                                   \
      : public BasicClonePtrMapper<styler_name##Styler>,                       \
        public virtual IStylerMapper {                                         \
    CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(styler_name##Styler,           \
                                                ClonePtr<styler_name##Styler>) \
                                                                               \
    ClonePtr<Styler> MapStylerFromXml(xml::XmlElementNode* node) override {    \
      return MapFromXml(node);                                                 \
    }                                                                          \
  };

CRU_DECLARE_STYLER_MAPPER(Border)
CRU_DECLARE_STYLER_MAPPER(ContentBrush)
CRU_DECLARE_STYLER_MAPPER(Cursor)
CRU_DECLARE_STYLER_MAPPER(Font)
CRU_DECLARE_STYLER_MAPPER(Margin)
CRU_DECLARE_STYLER_MAPPER(Padding)
CRU_DECLARE_STYLER_MAPPER(PreferredSize)

#undef CRU_DECLARE_STYLER_MAPPER
}  // namespace cru::ui::mapper::style
