#pragma once
#include "Mapper.h"
#include "cru/common/Base.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper {
class BrushMapper : public BasicRefMapper<platform::graphics::IBrush> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BrushMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::shared_ptr<platform::graphics::IBrush> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
