#pragma once

#include "XmlNode.hpp"

#include "cru/common/String.hpp"

#include <optional>

namespace cru::xml {
class XmlParser {
 public:
  explicit XmlParser(String xml);

  CRU_DELETE_COPY(XmlParser)
  CRU_DELETE_MOVE(XmlParser)

  ~XmlParser();

  XmlNode Parse();

 private:
  XmlNode DoParse();

 private:
  String xml_;

  std::optional<XmlNode> root_node_;
};
}  // namespace cru::xml
