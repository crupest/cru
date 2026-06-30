#pragma once

#include "../style/ApplyBorderStyleInfo.h"
#include "Base.h"

namespace cru::ui::datamodel {
/**
 * Example xml:
 * ```xml
 * <BorderStyle>
 *   <Thickness value="1" />
 *   <CornerRadius all="1" />
 *   <Brush><Color value=""/></Brush>
 *   <Brush name="foreground"><Color value="transparent"/></Brush>
 *   <Brush name="background"><Color value="#eeeeeeff"/></Brush>
 * </BorderStyle>
 * ```
 */
class CRU_UI_API BorderStyleDataType
    : public DataTypeBase<ui::style::ApplyBorderStyleInfo> {
 public:
  BorderStyleDataType();

 protected:
  bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;
  DataConvertResult<ui::style::ApplyBorderStyleInfo> DoConvertFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::datamodel
