#include "cru/xml/XmlNode.hpp"
#include <algorithm>

namespace cru::xml {

XmlElementNode* XmlNode::AsElement() {
  return static_cast<XmlElementNode*>(this);
}

XmlTextNode* XmlNode::AsText() { return static_cast<XmlTextNode*>(this); }

const XmlElementNode* XmlNode::AsElement() const {
  return static_cast<const XmlElementNode*>(this);
}

const XmlTextNode* XmlNode::AsText() const {
  return static_cast<const XmlTextNode*>(this);
}

XmlElementNode::~XmlElementNode() {
  for (auto child : children_) {
    delete child;
  }
}

void XmlElementNode::AddAttribute(String key, String value) {
  attributes_[std::move(key)] = std::move(value);
}

void XmlElementNode::AddChild(XmlNode* child) {
  Expects(child->GetParent() == nullptr);
  children_.push_back(child);
  child->parent_ = this;
}

Index XmlElementNode::GetChildElementCount() const {
  return std::count_if(
      children_.cbegin(), children_.cend(),
      [](xml::XmlNode* node) { return node->IsElementNode(); });
}

XmlElementNode* XmlElementNode::GetFirstChildElement() const {
  for (auto child : children_) {
    if (child->GetType() == XmlNode::Type::Element) {
      return child->AsElement();
    }
  }
  return nullptr;
}

XmlNode* XmlElementNode::Clone() const {
  XmlElementNode* node = new XmlElementNode(tag_, attributes_);

  for (auto child : children_) {
    node->AddChild(child->Clone());
  }

  return node;
}
}  // namespace cru::xml
