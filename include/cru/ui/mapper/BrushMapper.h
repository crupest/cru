#pragma once
#include "Mapper.h"
#include "cru/base/Base.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/base/xml/XmlNode.h"

namespace cru::ui::mapper {
class BrushMapper : public BasicSharedPtrMapper<platform::graphics::IBrush> {
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
