#include "cru/xml/XmlParser.hpp"
#include "cru/xml/XmlNode.hpp"

namespace cru::xml {
XmlElementNode* XmlParser::Parse() {
  if (!cache_) {
    cache_ = DoParse();
  }
  return static_cast<XmlElementNode*>(cache_->Clone());
}

char16_t XmlParser::Read1() {
  if (current_position_ >= xml_.size()) {
    throw XmlParsingException(u"Unexpected end of xml");
  }
  return xml_[current_position_++];
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
    throw XmlParsingException(u"Expected \".");
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
    switch (xml_[current_position_]) {
      case '<': {
        ++current_position_;

        if (Read1() == '/') {
        } else {
          ReadSpacesAndDiscard();

          String tag = ReadIdenitifier();

          XmlElementNode* node = new XmlElementNode(tag);

          while (true) {
            ReadSpacesAndDiscard();
            if (Read1() == '>') {
              break;
            } else {
              String attribute_name = ReadIdenitifier();

              ReadSpacesAndDiscard();

              if (Read1() != '=') {
                throw XmlParsingException(u"Expected '='");
              }

              ReadSpacesAndDiscard();

              String attribute_value = ReadAttributeString();

              node->AddAttribute(attribute_name, attribute_value);
            }
          }

          current_->AddChild(node);
          current_ = node;
        }

        break;
      }
    }
  }

  return pseudo_root_node_;
}
}  // namespace cru::xml
