#pragma once
#include "Mapper.h"

#include <cru/base/xml/XmlNode.h>
#include <cru/platform/graphics/Brush.h>

namespace cru::ui::mapper {
/**
 * Color element can be used as a brush.
 *
 * Or example xml:
 * ```xml
 * <Brush><Color value="black"/></Brush>
 * ```
 */
class BrushMapper : public BasicSharedPtrMapper<platform::graphics::IBrush> {
  CRU_UI_DECLARE_CAN_MAP_FROM_XML_ELEMENT(
      std::shared_ptr<platform::graphics::IBrush>)
};
}  // namespace cru::ui::mapper
