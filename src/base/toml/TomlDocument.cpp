#include "cru/base/toml/TomlDocument.h"

namespace cru::toml {
std::optional<std::string> TomlSection::GetValue(const std::string& key) const {
  auto it = values_.find(key);
  if (it == values_.end()) {
    return std::nullopt;
  }
  return it->second;
}

void TomlSection::SetValue(const std::string& key, std::string value) {
  values_[key] = std::move(value);
}

void TomlSection::DeleteValue(const std::string& key) { values_.erase(key); }

TomlSection* TomlDocument::GetSection(const std::string& name) {
  auto it = sections_.find(name);
  if (it == sections_.end()) {
    return nullptr;
  }
  return &it->second;
}

const TomlSection* TomlDocument::GetSection(const std::string& name) const {
  auto it = sections_.find(name);
  if (it == sections_.end()) {
    return nullptr;
  }
  return &it->second;
}

TomlSection* TomlDocument::GetSectionOrCreate(const std::string& name) {
  auto it = sections_.find(name);
  if (it == sections_.end()) {
    sections_[name] = TomlSection();
    return &sections_[name];
  }
  return &it->second;
}

void TomlDocument::SetSection(const std::string& name, TomlSection section) {
  sections_[name] = std::move(section);
}

void TomlDocument::DeleteSection(const std::string& name) {
  sections_.erase(name);
}

}  // namespace cru::toml
