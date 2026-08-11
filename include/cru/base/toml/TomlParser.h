#pragma once

#include "../Base.h"
#include "TomlDocument.h"

namespace cru::toml {
// A very simple and tolerant TOML parser.
class CRU_BASE_API TomlParsingException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_BASE_API TomlParser : public Object {
 public:
  explicit TomlParser(std::string input);

 public:
  TomlDocument Parse();

 private:
  std::string input_;
};
}  // namespace cru::toml
