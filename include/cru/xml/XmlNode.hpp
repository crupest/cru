#pragma once

#include "Base.hpp"

#include "cru/common/String.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace cru::xml {
class XmlElementNode;
class XmlTextNode;

class CRU_XML_API XmlNode {
  friend XmlElementNode;

 public:
  enum class Type { Text, Element };

 protected:
  explicit XmlNode(Type type) : type_(type) {}

 public:
  CRU_DELETE_COPY(XmlNode)
  CRU_DELETE_MOVE(XmlNode)

  virtual ~XmlNode() = default;

  Type GetType() const { return type_; }
  XmlElementNode* GetParent() const { return parent_; }

  virtual XmlNode* Clone() const = 0;

  XmlElementNode* AsElement();
  XmlTextNode* AsText();
  const XmlElementNode* AsElement() const;
  const XmlTextNode* AsText() const;

 private:
  const Type type_;
  XmlElementNode* parent_ = nullptr;
};

class CRU_XML_API XmlTextNode : public XmlNode {
 public:
  XmlTextNode() : XmlNode(Type::Text) {}
  explicit XmlTextNode(String text)
      : XmlNode(Type::Text), text_(std::move(text)) {}

  CRU_DELETE_COPY(XmlTextNode)
  CRU_DELETE_MOVE(XmlTextNode)

  ~XmlTextNode() override = default;

 public:
  String GetText() const { return text_; }
  void SetText(String text) { text_ = std::move(text); }

  XmlNode* Clone() const override { return new XmlTextNode(text_); }

 private:
  String text_;
};

class CRU_XML_API XmlElementNode : public XmlNode {
 public:
  XmlElementNode() : XmlNode(Type::Element) {}
  explicit XmlElementNode(String tag,
                          std::unordered_map<String, String> attributes = {})
      : XmlNode(Type::Element),
        tag_(std::move(tag)),
        attributes_(std::move(attributes)) {}

  CRU_DELETE_COPY(XmlElementNode)
  CRU_DELETE_MOVE(XmlElementNode)

  ~XmlElementNode() override;

 public:
  String GetTag() const { return tag_; }
  void SetTag(String tag) { tag_ = std::move(tag); }
  const std::unordered_map<String, String>& GetAttributes() const {
    return attributes_;
  }
  void SetAttributes(std::unordered_map<String, String> attributes) {
    attributes_ = std::move(attributes);
  }
  const std::vector<XmlNode*> GetChildren() const { return children_; }

  Index GetChildCount() const { return children_.size(); }
  String GetAttribute(const String& key) const { return attributes_.at(key); }
  std::optional<String> GetOptionalAttribute(const String& key) const {
    auto it = attributes_.find(key);
    if (it == attributes_.end()) {
      return std::nullopt;
    }

    return it->second;
  }
  XmlNode* GetChildAt(Index index) const { return children_[index]; }

  void AddAttribute(String key, String value);
  void AddChild(XmlNode* child);

  XmlNode* Clone() const override;

 private:
  String tag_;
  std::unordered_map<String, String> attributes_;
  std::vector<XmlNode*> children_;
};

}  // namespace cru::xml
