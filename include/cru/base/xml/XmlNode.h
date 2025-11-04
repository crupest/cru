#pragma once

#include "../Base.h"
#include "../StringUtil.h"

namespace cru::xml {
class XmlElementNode;
class XmlTextNode;
class XmlCommentNode;

class CRU_BASE_API XmlNode : public Object {
  friend XmlElementNode;

 public:
  enum class Type { Text, Element, Comment };

 protected:
  explicit XmlNode(Type type) : type_(type) {}

 public:
  Type GetType() const { return type_; }
  XmlElementNode* GetParent() const { return parent_; }

  virtual XmlNode* Clone() const = 0;

  bool IsTextNode() const { return type_ == Type::Text; }
  bool IsElementNode() const { return type_ == Type::Element; }
  bool IsCommentNode() const { return type_ == Type::Comment; }

  XmlElementNode* AsElement();
  XmlTextNode* AsText();
  XmlCommentNode* AsComment();
  const XmlElementNode* AsElement() const;
  const XmlTextNode* AsText() const;
  const XmlCommentNode* AsComment() const;

 private:
  const Type type_;
  XmlElementNode* parent_ = nullptr;
};

class CRU_BASE_API XmlTextNode : public XmlNode {
 public:
  XmlTextNode() : XmlNode(Type::Text) {}
  explicit XmlTextNode(std::string text)
      : XmlNode(Type::Text), text_(std::move(text)) {}

 public:
  std::string GetText() const { return text_; }
  void SetText(std::string text) { text_ = std::move(text); }

  XmlNode* Clone() const override { return new XmlTextNode(text_); }

 private:
  std::string text_;
};

class CRU_BASE_API XmlElementNode : public XmlNode {
 public:
  XmlElementNode() : XmlNode(Type::Element) {}
  explicit XmlElementNode(
      std::string tag,
      std::unordered_map<std::string, std::string> attributes = {})
      : XmlNode(Type::Element),
        tag_(std::move(tag)),
        attributes_(std::move(attributes)) {}

  ~XmlElementNode() override;

 public:
  std::string GetTag() const { return tag_; }
  void SetTag(std::string tag) { tag_ = std::move(tag); }
  const std::unordered_map<std::string, std::string>& GetAttributes() const {
    return attributes_;
  }
  void SetAttributes(std::unordered_map<std::string, std::string> attributes) {
    attributes_ = std::move(attributes);
  }
  const std::vector<XmlNode*> GetChildren() const { return children_; }

  Index GetChildCount() const { return children_.size(); }
  std::string GetAttributeValue(const std::string& key) const {
    return attributes_.at(key);
  }
  std::string GetAttributeValueCaseInsensitive(const std::string& key) const {
    return *GetOptionalAttributeValueCaseInsensitive(key);
  }
  std::optional<std::string> GetOptionalAttributeValue(
      const std::string& key) const {
    auto it = attributes_.find(key);
    if (it == attributes_.end()) {
      return std::nullopt;
    }

    return it->second;
  }
  std::optional<std::string> GetOptionalAttributeValueCaseInsensitive(
      const std::string& key) const {
    for (auto it = attributes_.begin(); it != attributes_.end(); ++it) {
      if (cru::string::CaseInsensitiveCompare(it->first, key) == 0) {
        return it->second;
      }
    }

    return std::nullopt;
  }

  XmlNode* GetChildAt(Index index) const { return children_[index]; }

  void AddAttribute(std::string key, std::string value);
  void AddChild(XmlNode* child);

  Index GetChildElementCount() const;
  XmlElementNode* GetFirstChildElement() const;

  XmlNode* Clone() const override;

 private:
  std::string tag_;
  std::unordered_map<std::string, std::string> attributes_;
  std::vector<XmlNode*> children_;
};

class CRU_BASE_API XmlCommentNode : public XmlNode {
 public:
  XmlCommentNode() : XmlNode(Type::Comment) {}
  explicit XmlCommentNode(std::string text)
      : XmlNode(Type::Comment), text_(std::move(text)) {}

  std::string GetText() const { return text_; }
  void SetText(std::string text) { text_ = std::move(text); }

  XmlNode* Clone() const override;

 private:
  std::string text_;
};
}  // namespace cru::xml
