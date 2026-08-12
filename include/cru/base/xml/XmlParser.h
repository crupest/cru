#pragma once

#include "../Base.h"
#include "XmlNode.h"

namespace cru::xml {
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
