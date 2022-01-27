#pragma once
#include "Mapper.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/graphics/Font.hpp"

namespace cru::ui::mapper {
class FontMapper : public BasicRefMapper<platform::graphics::IFont> {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(FontMapper)

 public:
  bool SupportMapFromXml() override { return true; }
  bool XmlElementIsOfThisType(xml::XmlElementNode* node) override;

 protected:
  std::shared_ptr<platform::graphics::IFont> DoMapFromXml(
      xml::XmlElementNode* node) override;
};
}  // namespace cru::ui::mapper
