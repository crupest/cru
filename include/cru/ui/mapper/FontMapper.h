#pragma once
#include "Mapper.h"
#include <cru/Base.h>
#include "cru/graphics/Font.h"

namespace cru::ui::mapper {
class FontMapper : public BasicSharedPtrMapper<graphics::IFont> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(FontMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::shared_ptr<graphics::IFont> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
