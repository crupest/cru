#include "cru/xml/XmlParser.h"
#include "cru/xml/XmlNode.h"

namespace cru::xml {
XmlParser::XmlParser(String xml) : xml_(std::move(xml)) {}

XmlParser::~XmlParser() { delete pseudo_root_node_; }

XmlElementNode* XmlParser::Parse() {
  if (!cache_) {
    cache_ = DoParse();
  }
  return static_cast<XmlElementNode*>(cache_->Clone());
}

char16_t XmlParser::Read1() {
  if (current_position_ >= xml_.size()) {
    throw XmlParsingException("Unexpected end of xml");
  }
  return xml_[current_position_++];
}

String XmlParser::ReadWithoutAdvance(int count) {
  if (current_position_ + count > xml_.size()) {
    count = xml_.size() - current_position_;
  }
  return xml_.substr(current_position_, count);
}

void XmlParser::ReadSpacesAndDiscard() {
  while (current_position_ < xml_.size() &&
         (xml_[current_position_] == ' ' || xml_[current_position_] == '\t' ||
          xml_[current_position_] == '\n' || xml_[current_position_] == '\r')) {
    ++current_position_;
  }
}

String XmlParser::ReadSpaces() {
  String spaces;
  while (current_position_ < xml_.size() &&
         (xml_[current_position_] == ' ' || xml_[current_position_] == '\t' ||
          xml_[current_position_] == '\n' || xml_[current_position_] == '\r')) {
    spaces += xml_[current_position_];
    ++current_position_;
  }
  return spaces;
}

String XmlParser::ReadIdenitifier() {
  String identifier;
  while (current_position_ < xml_.size() &&
         (xml_[current_position_] >= 'a' && xml_[current_position_] <= 'z' ||
          xml_[current_position_] >= 'A' && xml_[current_position_] <= 'Z' ||
          xml_[current_position_] >= '0' && xml_[current_position_] <= '9' ||
          xml_[current_position_] == '_')) {
    identifier += xml_[current_position_];
    ++current_position_;
  }
  return identifier;
}

String XmlParser::ReadAttributeString() {
  if (Read1() != '"') {
    throw XmlParsingException("Expected \".");
  }

  String string;

  while (true) {
    char16_t c = Read1();

    if (c == '"') {
      break;
    }

    string += c;
  }

  return string;
}

XmlElementNode* XmlParser::DoParse() {
  while (current_position_ < xml_.size()) {
    ReadSpacesAndDiscard();

    if (current_position_ == xml_.size()) {
      break;
    }

    if (ReadWithoutAdvance() == u"<") {
      current_position_ += 1;

      if (ReadWithoutAdvance() == u"/") {
        current_position_ += 1;

        ReadSpacesAndDiscard();

        String tag = ReadIdenitifier();

        if (tag != current_->GetTag()) {
          throw XmlParsingException("Tag mismatch.");
        }

        ReadSpacesAndDiscard();

        if (Read1() != '>') {
          throw XmlParsingException("Expected >.");
        }

        current_ = current_->GetParent();
      } else if (ReadWithoutAdvance(3) == u"!--") {
        current_position_ += 3;

        String text;
        while (true) {
          auto str = ReadWithoutAdvance(3);
          if (str == u"-->") break;
          if (str.empty()) throw XmlParsingException("Unexpected end of xml");
          text += Read1();
        }

        current_position_ += 3;
        current_->AddChild(new XmlCommentNode(text.Trim()));
      } else {
        ReadSpacesAndDiscard();

        String tag = ReadIdenitifier();

        XmlElementNode* node = new XmlElementNode(tag);

        bool is_self_closing = false;

        while (true) {
          ReadSpacesAndDiscard();
          auto c = ReadWithoutAdvance();
          if (c == u">") {
            current_position_ += 1;
            break;
          } else if (c == u"/") {
            current_position_ += 1;

            if (Read1() != '>') {
              throw XmlParsingException("Expected >.");
            }

            is_self_closing = true;
            break;
          } else {
            String attribute_name = ReadIdenitifier();

            ReadSpacesAndDiscard();

            if (Read1() != '=') {
              throw XmlParsingException("Expected '='");
            }

            ReadSpacesAndDiscard();

            String attribute_value = ReadAttributeString();

            node->AddAttribute(attribute_name, attribute_value);
          }
        }

        current_->AddChild(node);

        if (!is_self_closing) {
          current_ = node;
        }
      }

    } else {
      String text;

      while (ReadWithoutAdvance() != u"<") {
        char16_t c = Read1();

        text += c;
      }

      if (!text.empty()) current_->AddChild(new XmlTextNode(text.TrimEnd()));
    }
  }

  if (current_ != pseudo_root_node_) {
    throw XmlParsingException("Unexpected end of xml");
  }

  if (pseudo_root_node_->GetChildren().size() != 1) {
    throw XmlParsingException("Expected 1 node as root.");
  }

  return static_cast<XmlElementNode*>(pseudo_root_node_->GetChildren()[0]);
}
}  // namespace cru::xml
