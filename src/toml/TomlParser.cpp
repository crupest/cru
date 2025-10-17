#include "cru/toml/TomlParser.h"
#include "cru/toml/TomlDocument.h"

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
  std::vector<String> lines = input_.SplitToLines(true);

  String current_section_name;

  for (auto& line : lines) {
    line.Trim();
    if (line.StartWith(u"[") && line.EndWith(u"]")) {
      current_section_name = line.substr(1, line.size() - 2);
    } else if (line.StartWith(u"#")) {
      // Ignore comments.
    } else {
      auto equal_index = line.Find(u'=');

      if (equal_index == -1) {
        throw TomlParsingException("Invalid TOML line: " + line.ToUtf8());
      }

      auto key = line.substr(0, equal_index).Trim();
      auto value = line.substr(equal_index + 1).Trim();

      document.GetSectionOrCreate(current_section_name)->SetValue(key, value);
    }
  }
}
}  // namespace cru::toml
