#include "cru/xml/XmlNode.hpp"

namespace cru::xml {
bool operator==(const XmlNode& lhs, const XmlNode& rhs) {
  return lhs.GetType() == rhs.GetType() && lhs.GetText() == rhs.GetText() &&
         lhs.GetTag() == rhs.GetTag() &&
         lhs.GetAttributes() == rhs.GetAttributes() &&
         lhs.GetChildren() == rhs.GetChildren();
}

bool operator!=(const XmlNode& lhs, const XmlNode& rhs) {
  return !(lhs == rhs);
}
}  // namespace cru::xml
