#pragma once

#include "cru/common/Exception.hpp"
#include "cru/toml/TomlDocument.hpp"

#include <optional>

namespace cru::toml {
// A very simple and tolerant TOML parser.
class CRU_TOML_API TomlParsingException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_TOML_API TomlParser {
 public:
  explicit TomlParser(String input);

  CRU_DELETE_COPY(TomlParser)
  CRU_DELETE_MOVE(TomlParser)

  ~TomlParser();

 public:
  TomlDocument Parse();

 private:
  // The document is empty to begin with.
  void DoParse(TomlDocument& document);

 private:
  String input_;

  std::optional<TomlDocument> cache_;
};
}  // namespace cru::toml
