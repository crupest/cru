#pragma once

#include "Base.h"
#include "Dictionary.h"

#include <string_view>

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
  using AttrDict = Dictionary<std::string, std::string>;

  explicit XmlElementNode(std::string tag,
                          Dictionary<std::string, std::string> attributes = {});

  ~XmlElementNode() override;

 public:
  std::string GetTag() const { return tag_; }
  bool HasTag(std::string_view tag, bool case_sensitive = false);
  void SetTag(std::string tag);
  const std::vector<XmlNode*>& GetChildren() const { return children_; }
  Index GetChildCount() const { return children_.size(); }
  XmlNode* GetChildAt(Index index) const { return children_[index]; }
  Index GetChildElementCount() const;
  XmlElementNode* GetFirstChildElement() const;

  void AddChild(XmlNode* child);
  XmlNode* RemoveChildAt(Index index);

  AttrDict& GetAttributes() { return attributes_; }
  const AttrDict& GetAttributes() const { return attributes_; }

  std::optional<std::string_view> GetOptionalAttributeValue(
      std::string_view key, bool case_sensitive = false) const;

  XmlNode* Clone() const override;

 private:
  std::string tag_;
  AttrDict attributes_;
  std::vector<XmlNode*> children_;
};

class CRU_BASE_API XmlCommentNode : public XmlNode {
 public:
  explicit XmlCommentNode(std::string text)
      : XmlNode(Type::Comment), text_(std::move(text)) {}

  std::string GetText() const { return text_; }
  void SetText(std::string text) { text_ = std::move(text); }

  XmlNode* Clone() const override;

 private:
  std::string text_;
};

class CRU_BASE_API XmlParsingException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_BASE_API XmlParser : public Object {
 public:
  explicit XmlParser(std::string_view source);

  XmlElementNode* Parse();

 private:
  bool IsEnd();
  char Read1();
  std::string_view Peek(int count = 1);
  std::string_view ReadSpaces();
  std::string_view ReadIdentifier();
  std::string_view ReadAttributeString();

 private:
  std::string_view source_;
  int current_position_;
};
}  // namespace cru::xml
