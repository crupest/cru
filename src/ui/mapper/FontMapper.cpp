#include "cru/ui/mapper/FontMapper.h"
#include "../Helper.h"
#include "cru/base/String.h"
#include "cru/base/StringUtil.h"
#include "cru/platform/graphics/Factory.h"

namespace cru::ui::mapper {
bool FontMapper::XmlElementIsOfThisType(xml::XmlElementNode* node) {
  return cru::string::CaseInsensitiveCompare(node->GetTag(), "font") == 0;
}

std::shared_ptr<platform::graphics::IFont> FontMapper::DoMapFromXml(
    xml::XmlElementNode* node) {
  auto font_family_attr = node->GetOptionalAttributeValue("family");
  auto font_size_attr = node->GetOptionalAttributeValue("size");

  auto font_family = font_family_attr.value_or("");
  auto font_size =
      font_size_attr ? cru::string::ParseToNumber<float>(*font_size_attr).value
                     : 24.0f;

  return GetGraphicsFactory()->CreateFont(font_family, font_size);
}
}  // namespace cru::ui::mapper
