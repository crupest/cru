#include "cru/xml/XmlParser.hpp"

namespace cru::xml {
XmlNode XmlParser::Parse() {
  if (!root_node_) {
    root_node_ = DoParse();
  }
  return *root_node_;
}

XmlNode XmlParser::DoParse() {
  XmlNode root(XmlNode::Type::Element);
  XmlNode* current = &root;
  int current_position = 0;

  // TODO: Implement this.
  while (current_position < xml_.size()) {
    switch (xml_[current_position]) {
      case '<': {
        break;
      }
    }
  }

  return root;
}
}  // namespace cru::xml
