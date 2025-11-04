#include "cru/base/xml/XmlNode.h"

#include <algorithm>

namespace cru::xml {

XmlElementNode* XmlNode::AsElement() {
  return static_cast<XmlElementNode*>(this);
}

XmlTextNode* XmlNode::AsText() { return static_cast<XmlTextNode*>(this); }

XmlCommentNode* XmlNode::AsComment() {
  return static_cast<XmlCommentNode*>(this);
}

const XmlElementNode* XmlNode::AsElement() const {
  return static_cast<const XmlElementNode*>(this);
}

const XmlTextNode* XmlNode::AsText() const {
  return static_cast<const XmlTextNode*>(this);
}

const XmlCommentNode* XmlNode::AsComment() const {
  return static_cast<const XmlCommentNode*>(this);
}

XmlElementNode::~XmlElementNode() {
  for (auto child : children_) {
    delete child;
  }
}

void XmlElementNode::AddAttribute(std::string key, std::string value) {
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

XmlNode* XmlCommentNode::Clone() const {
  XmlCommentNode* node = new XmlCommentNode(text_);

  return node;
}
}  // namespace cru::xml
