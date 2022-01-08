#include "cru/toml/TomlParser.hpp"
#include "cru/toml/TomlDocument.hpp"

namespace cru::toml {
TomlParser::TomlParser(String input) : input_(std::move(input)) {}

TomlParser::~TomlParser() = default;

TomlDocument TomlParser::Parse() {
  if (cache_) {
    return *cache_;
  }

  cache_ = TomlDocument();
  DoParse(*cache_);
  return *cache_;
}

void TomlParser::DoParse(TomlDocument& document) {
  // TODO: Implement this.
}
}  // namespace cru::toml
