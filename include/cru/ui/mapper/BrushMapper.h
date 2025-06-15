#pragma once
#include "Mapper.h"
#include <cru/Base.h>
#include "cru/graphics/Brush.h"
#include "cru/xml/XmlNode.h"

namespace cru::ui::mapper {
class BrushMapper : public BasicSharedPtrMapper<graphics::IBrush> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(BrushMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::shared_ptr<graphics::IBrush> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
