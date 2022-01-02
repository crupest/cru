#pragma once

#include "XmlNode.hpp"

#include "cru/common/Exception.hpp"
#include "cru/common/String.hpp"

#include <optional>

namespace cru::xml {
class XmlParsingException : public Exception {
 public:
  using Exception::Exception;
};

class XmlParser {
 public:
  explicit XmlParser(String xml);

  CRU_DELETE_COPY(XmlParser)
  CRU_DELETE_MOVE(XmlParser)

  ~XmlParser();

  XmlElementNode* Parse();

 private:
  XmlElementNode* DoParse();

  char16_t Read1();
  String ReadWithoutAdvance(int count = 1);
  void ReadSpacesAndDiscard();
  String ReadSpaces();
  String ReadIdenitifier();
  String ReadAttributeString();

 private:
  String xml_;

  XmlElementNode* cache_ = nullptr;

  // Consider the while file enclosed by a single tag called $root.
  XmlElementNode* pseudo_root_node_ = new XmlElementNode(u"$root");
  XmlElementNode* current_ = pseudo_root_node_;
  int current_position_ = 0;
};
}  // namespace cru::xml
