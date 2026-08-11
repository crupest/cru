#include "cru/base/toml/TomlDocument.h"

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
}  // namespace cru::toml
