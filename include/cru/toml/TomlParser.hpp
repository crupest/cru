#pragma once

#include "cru/toml/TomlDocument.hpp"

#include <optional>

namespace cru::toml {
class TomlParser {
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
