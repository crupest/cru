#include "cru/base/Xml.h"
#include "cru/base/StringUtil.h"

#include <cassert>
#include <memory>

namespace cru::xml {
XmlElementNode* XmlNode::AsElement() {
  return IsElementNode() ? static_cast<XmlElementNode*>(this) : nullptr;
}

XmlTextNode* XmlNode::AsText() {
  return IsTextNode() ? static_cast<XmlTextNode*>(this) : nullptr;
}

XmlCommentNode* XmlNode::AsComment() {
  return IsCommentNode() ? static_cast<XmlCommentNode*>(this) : nullptr;
}

const XmlElementNode* XmlNode::AsElement() const {
  return IsElementNode() ? static_cast<const XmlElementNode*>(this) : nullptr;
}

const XmlTextNode* XmlNode::AsText() const {
  return IsTextNode() ? static_cast<const XmlTextNode*>(this) : nullptr;
}

const XmlCommentNode* XmlNode::AsComment() const {
  return IsCommentNode() ? static_cast<const XmlCommentNode*>(this) : nullptr;
}

XmlElementNode::XmlElementNode(std::string tag,
                               Dictionary<std::string, std::string> attributes)
    : XmlNode(Type::Element),
      tag_(std::move(tag)),
      attributes_(std::move(attributes)) {}

XmlElementNode::~XmlElementNode() {
  for (auto child : children_) {
    delete child;
  }
}

bool XmlElementNode::HasTag(std::string_view tag, bool case_sensitive) {
  return case_sensitive ? tag_ == tag
                        : cru::string::CaseInsensitiveEqual(tag_, tag);
}

void XmlElementNode::SetTag(std::string tag) { tag_ = std::move(tag); }

Index XmlElementNode::GetChildElementCount() const {
  return std::ranges::count_if(
      children_, [](xml::XmlNode* node) { return node->IsElementNode(); });
}

XmlElementNode* XmlElementNode::GetFirstChildElement() const {
  for (auto child : children_) {
    if (child->GetType() == XmlNode::Type::Element) {
      return child->AsElement();
    }
  }
  return nullptr;
}

XmlNode* XmlElementNode::RemoveChildAt(Index index) {
  auto child = children_[index];
  children_.erase(children_.begin() + index);
  child->parent_ = nullptr;
  return child;
}

void XmlElementNode::AddChild(XmlNode* child) {
  assert(child->GetParent() == nullptr);
  children_.push_back(child);
  child->parent_ = this;
}

std::optional<std::string_view> XmlElementNode::GetOptionalAttributeValue(
    std::string_view key, bool case_sensitive) const {
  for (const auto& [k, v] : attributes_) {
    if (case_sensitive ? k == key : cru::string::CaseInsensitiveEqual(k, key)) {
      return v;
    }
  }
  return std::nullopt;
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

XmlParser::XmlParser(std::string_view source) : source_(std::move(source)) {}

bool XmlParser::IsEnd() { return current_position_ >= source_.size(); }

char XmlParser::Read1() {
  if (IsEnd()) {
    throw XmlParsingException("Unexpected end of xml");
  }
  return source_[current_position_++];
}

std::string_view XmlParser::Peek(int count) {
  if (current_position_ + count > source_.size()) {
    count = source_.size() - current_position_;
  }
  return source_.substr(current_position_, count);
}

std::string_view XmlParser::ReadSpaces() {
  auto old_position = current_position_;
  while (!IsEnd() && (source_[current_position_] == ' ' ||
                      source_[current_position_] == '\t' ||
                      source_[current_position_] == '\n' ||
                      source_[current_position_] == '\r')) {
    ++current_position_;
  }
  return source_.substr(old_position, current_position_ - old_position);
}

std::string_view XmlParser::ReadIdentifier() {
  auto old_position = current_position_;
  while (
      current_position_ < source_.size() &&
      (source_[current_position_] >= 'a' && source_[current_position_] <= 'z' ||
       source_[current_position_] >= 'A' && source_[current_position_] <= 'Z' ||
       source_[current_position_] >= '0' && source_[current_position_] <= '9' ||
       source_[current_position_] == '_')) {
    ++current_position_;
  }
  return source_.substr(old_position, current_position_ - old_position);
}

std::string_view XmlParser::ReadAttributeString() {
  if (Read1() != '"') {
    throw XmlParsingException("Expected \".");
  }

  auto old_position = current_position_;

  while (true) {
    char16_t c = Read1();

    if (c == '"') {
      break;
    }
  }

  return source_.substr(old_position, current_position_ - old_position - 1);
}

XmlElementNode* XmlParser::Parse() {
  current_position_ = 0;

  // Consider the while file enclosed by a single tag called $root.
  std::unique_ptr<XmlElementNode> pseudo_root_node_(
      new XmlElementNode("$root"));
  XmlElementNode* current_ = pseudo_root_node_.get();

  while (current_position_ < source_.size()) {
    ReadSpaces();

    if (current_position_ == source_.size()) {
      break;
    }

    if (Peek() == "<") {
      current_position_ += 1;

      if (Peek() == "/") {
        current_position_ += 1;

        ReadSpaces();

        auto tag = ReadIdentifier();

        if (tag != current_->GetTag()) {
          throw XmlParsingException("Tag mismatch.");
        }

        ReadSpaces();

        if (Read1() != '>') {
          throw XmlParsingException("Expected >.");
        }

        current_ = current_->GetParent();
      } else if (Peek(3) == "!--") {
        current_position_ += 3;

        std::string text;
        while (true) {
          auto str = Peek(3);
          if (str == "-->") break;
          if (str.empty()) throw XmlParsingException("Unexpected end of xml");
          text += Read1();
        }

        current_position_ += 3;
        current_->AddChild(new XmlCommentNode(cru::string::Trim(text)));
      } else {
        ReadSpaces();

        auto tag = ReadIdentifier();

        XmlElementNode* node = new XmlElementNode(std::string(tag));

        bool is_self_closing = false;

        while (true) {
          ReadSpaces();
          auto c = Peek();
          if (c == ">") {
            current_position_ += 1;
            break;
          } else if (c == "/") {
            current_position_ += 1;

            if (Read1() != '>') {
              throw XmlParsingException("Expected >.");
            }

            is_self_closing = true;
            break;
          } else {
            auto attribute_name = ReadIdentifier();

            ReadSpaces();

            if (Read1() != '=') {
              throw XmlParsingException("Expected '='");
            }

            ReadSpaces();

            auto attribute_value = ReadAttributeString();

            node->GetAttributes().emplace(attribute_name, attribute_value);
          }
        }

        current_->AddChild(node);

        if (!is_self_closing) {
          current_ = node;
        }
      }

    } else {
      std::string text;

      while (Peek() != "<") {
        char16_t c = Read1();

        text += c;
      }

      if (!text.empty())
        current_->AddChild(new XmlTextNode(cru::string::TrimEnd(text)));
    }
  }

  if (current_ != pseudo_root_node_.get()) {
    throw XmlParsingException("Unexpected end of xml");
  }

  if (pseudo_root_node_->GetChildren().size() != 1) {
    throw XmlParsingException("Expected 1 node as root.");
  }

  if (!pseudo_root_node_->GetChildAt(0)->IsElementNode()) {
    throw XmlParsingException("Root node must be an element.");
  }

  return pseudo_root_node_->RemoveChildAt(0)->AsElement();
}
}  // namespace cru::xml
