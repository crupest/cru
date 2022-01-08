#include "cru/toml/TomlDocument.hpp"

namespace cru::toml {
std::optional<String> TomlSection::GetValue(const String& key) const {
  auto it = values_.find(key);
  if (it == values_.end()) {
    return std::nullopt;
  }
  return it->second;
}

void TomlSection::SetValue(const String& key, String value) {
  values_[key] = std::move(value);
}

void TomlSection::DeleteValue(const String& key) { values_.erase(key); }

TomlSection* TomlDocument::GetSection(const String& name) {
  auto it = sections_.find(name);
  if (it == sections_.end()) {
    return nullptr;
  }
  return &it->second;
}

const TomlSection* TomlDocument::GetSection(const String& name) const {
  auto it = sections_.find(name);
  if (it == sections_.end()) {
    return nullptr;
  }
  return &it->second;
}

TomlSection* TomlDocument::GetSectionOrCreate(const String& name) {
  auto it = sections_.find(name);
  if (it == sections_.end()) {
    sections_[name] = TomlSection();
    return &sections_[name];
  }
  return &it->second;
}

void TomlDocument::SetSection(const String& name, TomlSection section) {
  sections_[name] = std::move(section);
}

void TomlDocument::DeleteSection(const String& name) { sections_.erase(name); }

}  // namespace cru::toml
