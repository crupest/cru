#pragma once

#include "cru/common/String.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace cru::xml {
class XmlNode {
 public:
  enum class Type { Text, Element };

  static XmlNode CreateText(String text) {
    XmlNode node(Type::Text);
    node.SetText(std::move(text));
    return node;
  }

  static XmlNode CreateElement(
      String tag, std::unordered_map<String, String> attributes = {},
      std::vector<XmlNode> children = {}) {
    XmlNode node(Type::Element);
    node.SetTag(std::move(tag));
    node.SetAttributes(std::move(attributes));
    node.SetChildren(std::move(children));
    return node;
  }

  explicit XmlNode(Type type) : type_(type) {}

  CRU_DEFAULT_COPY(XmlNode)
  CRU_DEFAULT_MOVE(XmlNode)

  ~XmlNode() = default;

  Type GetType() const { return type_; }
  String GetText() const { return text_; }
  String GetTag() const { return tag_; }
  std::unordered_map<String, String> GetAttributes() { return attributes_; }
  const std::unordered_map<String, String>& GetAttributes() const {
    return attributes_;
  }
  std::vector<XmlNode>& GetChildren() { return children_; }
  const std::vector<XmlNode>& GetChildren() const { return children_; }

  void SetType(Type type) { type_ = type; }
  void SetText(String text) { text_ = std::move(text); }
  void SetTag(String tag) { tag_ = std::move(tag); }
  void SetAttributes(std::unordered_map<String, String> attributes) {
    attributes_ = std::move(attributes);
  }
  void SetChildren(std::vector<XmlNode> children) {
    children_ = std::move(children);
  }

 private:
  Type type_;
  String text_;
  String tag_;
  std::unordered_map<String, String> attributes_;
  std::vector<XmlNode> children_;
};

bool operator==(const XmlNode& lhs, const XmlNode& rhs);
bool operator!=(const XmlNode& lhs, const XmlNode& rhs);
}  // namespace cru::xml
