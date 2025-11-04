#pragma once

#include "../Base.h"
#include "XmlNode.h"

namespace cru::xml {
class CRU_BASE_API XmlParsingException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_BASE_API XmlParser {
 public:
  explicit XmlParser(std::string xml);

  CRU_DELETE_COPY(XmlParser)
  CRU_DELETE_MOVE(XmlParser)

  ~XmlParser();

  XmlElementNode* Parse();

 private:
  XmlElementNode* DoParse();

  char16_t Read1();
  std::string ReadWithoutAdvance(int count = 1);
  void ReadSpacesAndDiscard();
  std::string ReadSpaces();
  std::string ReadIdenitifier();
  std::string ReadAttributeString();

 private:
  std::string xml_;

  XmlElementNode* cache_ = nullptr;

  // Consider the while file enclosed by a single tag called $root.
  XmlElementNode* pseudo_root_node_ = new XmlElementNode("$root");
  XmlElementNode* current_ = pseudo_root_node_;
  int current_position_ = 0;
};
}  // namespace cru::xml
