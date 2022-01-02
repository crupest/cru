#include "cru/xml/XmlNode.hpp"

namespace cru::xml {
XmlElementNode::~XmlElementNode() {
  for (auto child : children_) {
    delete child;
  }
}

void XmlElementNode::AddAttribute(String key, String value) {
  attributes_[std::move(key)] = std::move(value);
}

void XmlElementNode::AddChild(XmlNode* child) {
  assert(child->GetParent() == nullptr);
  children_.push_back(child);
  child->parent_ = this;
}

XmlNode* XmlElementNode::Clone() const {
  XmlElementNode* node = new XmlElementNode(tag_, attributes_);

  for (auto child : children_) {
    node->AddChild(child->Clone());
  }

  return node;
}
}  // namespace cru::xml
