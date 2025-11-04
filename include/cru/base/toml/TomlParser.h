#pragma once

#include "../Base.h"
#include "TomlDocument.h"

#include <optional>

namespace cru::toml {
// A very simple and tolerant TOML parser.
class CRU_BASE_API TomlParsingException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_BASE_API TomlParser : public Object {
 public:
  explicit TomlParser(std::string input);
  ~TomlParser();

 public:
  TomlDocument Parse();

 private:
  // The document is empty to begin with.
  void DoParse(TomlDocument& document);

 private:
  std::string input_;

  std::optional<TomlDocument> cache_;
};
}  // namespace cru::toml
