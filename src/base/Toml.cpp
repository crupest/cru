#include "cru/base/Toml.h"

#include "cru/base/StringUtil.h"

#include <algorithm>

namespace cru::toml {

namespace {
template <typename T>
auto FindIteratorByKey(T& values, std::string_view key) {
  return std::ranges::find_if(
      values, [key](const auto& pair) { return pair.first == key; });
}
}  // namespace

bool TomlSection::HasKey(std::string_view key) const {
  return FindIteratorByKey(values_, key) != values_.end();
}

json::JsonValue& TomlSection::GetValue(std::string_view key) {
  auto it = FindIteratorByKey(values_, key);
  if (it == values_.end()) {
    throw Exception("Key not found: " + std::string(key));
  }
  return it->second;
}

const json::JsonValue& TomlSection::GetValue(std::string_view key) const {
  auto it = FindIteratorByKey(values_, key);
  if (it == values_.end()) {
    throw Exception("Key not found: " + std::string(key));
  }
  return it->second;
}

void TomlSection::SetValue(std::string_view key, json::JsonValue value) {
  auto it = FindIteratorByKey(values_, key);
  if (it == values_.end()) {
    values_.emplace_back(key, std::move(value));
  } else {
    it->second = std::move(value);
  }
}

bool TomlSection::DeleteValue(std::string_view key) {
  auto it = FindIteratorByKey(values_, key);
  if (it != values_.end()) {
    values_.erase(it);
    return true;
  }
  return false;
}

bool TomlDocument::HasSection(std::string_view name) const {
  return FindIteratorByKey(sections_, name) != sections_.end();
}

TomlSection& TomlDocument::GetSection(std::string_view name) {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    throw Exception("Section not found: " + std::string(name));
  }
  return it->second;
}

const TomlSection& TomlDocument::GetSection(std::string_view name) const {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    throw Exception("Section not found: " + std::string(name));
  }
  return it->second;
}

TomlSection& TomlDocument::GetSectionOrCreate(std::string_view name) {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    sections_.emplace_back(name, TomlSection());
    return sections_.back().second;
  }
  return it->second;
}

void TomlDocument::SetSection(std::string_view name, TomlSection section) {
  auto it = FindIteratorByKey(sections_, name);
  if (it == sections_.end()) {
    sections_.emplace_back(name, std::move(section));
  } else {
    it->second = std::move(section);
  }
}

bool TomlDocument::DeleteSection(std::string_view name) {
  auto it = FindIteratorByKey(sections_, name);
  if (it != sections_.end()) {
    sections_.erase(it);
    return true;
  }
  return false;
}

TomlParser::TomlParser(std::string input) : input_(std::move(input)) {}

TomlDocument TomlParser::Parse() {
  TomlDocument document;

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

      document.GetSectionOrCreate(current_section_name).SetValue(key, value);
    }
  }
  return document;
}
}  // namespace cru::toml
