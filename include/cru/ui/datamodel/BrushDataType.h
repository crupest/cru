#pragma once

#include "Base.h"

#include <cru/platform/graphics/Brush.h>

namespace cru::ui::datamodel {
/**
 * Color element can be used as a brush.
 *
 * Or example xml:
 * ```xml
 * <Brush><Color value="black"/></Brush>
 * ```
 */
class CRU_UI_API BrushDataType
    : public SharedPtrDataTypeBase<platform::graphics::IBrush> {
 public:
  BrushDataType();

 protected:
  bool DoXmlIsOfThisType(xml::XmlElementNode* node) override;
  DataConvertResult<std::shared_ptr<platform::graphics::IBrush>>
  DoConvertFromXml(xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::datamodel
