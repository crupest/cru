#include "cru/base/toml/TomlParser.h"
#include "cru/base/StringUtil.h"
#include "cru/base/toml/TomlDocument.h"

namespace cru::toml {
TomlParser::TomlParser(std::string input) : input_(std::move(input)) {}

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
  std::vector<std::string> lines =
      cru::string::Split(input_, "\n", cru::string::SplitOptions::RemoveSpace);

  std::string current_section_name;

  for (auto& line : lines) {
    line = cru::string::Trim(line);
    if (line.starts_with("[") && line.ends_with("]")) {
      current_section_name = line.substr(1, line.size() - 2);
    } else if (line.starts_with("#")) {
      // Ignore comments.
    } else {
      auto equal_index = line.find('=');

      if (equal_index == std::string::npos) {
        throw TomlParsingException("Invalid TOML line: " + line);
      }

      auto key = cru::string::Trim(line.substr(0, equal_index));
      auto value = cru::string::Trim(line.substr(equal_index + 1));

      document.GetSectionOrCreate(current_section_name)->SetValue(key, value);
    }
  }
}
}  // namespace cru::toml
