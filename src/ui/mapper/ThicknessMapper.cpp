#include "cru/ui/mapper/ThicknessMapper.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::ui::mapper {
bool ThicknessMapper::XmlElementIsOfThisType(xml::XmlElementNode *node) {
  if (node->GetTag() == u"Thickness") {
    return true;
  }
  return false;
}
}  // namespace cru::ui::mapper
