#pragma once
#include "../style/ApplyBorderStyleInfo.h"
#include "Mapper.h"

#include <cru/base/xml/XmlNode.h>

namespace cru::ui::mapper {
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
class CRU_UI_API BorderStyleMapper
    : public BasicMapper<ui::style::ApplyBorderStyleInfo> {
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT_TAG(BorderStyle,
                                              ui::style::ApplyBorderStyleInfo)
};
}  // namespace cru::ui::mapper
