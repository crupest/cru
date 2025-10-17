#pragma once

#include "Base.h"

#include "cru/base/Base.h"

#include <optional>
#include <unordered_map>

namespace cru::toml {
class CRU_TOML_API TomlSection {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(TomlSection)
  CRU_DEFAULT_COPY(TomlSection)
  CRU_DEFAULT_MOVE(TomlSection)

 public:
  std::optional<std::string> GetValue(const std::string& key) const;
  void SetValue(const std::string& key, std::string value);
  void DeleteValue(const std::string& key);

  auto begin() { return values_.begin(); }
  auto end() { return values_.end(); }
  auto begin() const { return values_.begin(); }
  auto end() const { return values_.end(); }
  auto cbegin() const { return values_.cbegin(); }
  auto cend() const { return values_.cend(); }

 private:
  std::unordered_map<std::string, std::string> values_;
};

class CRU_TOML_API TomlDocument {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(TomlDocument)
  CRU_DEFAULT_COPY(TomlDocument)
  CRU_DEFAULT_MOVE(TomlDocument)

 public:
  TomlSection* GetSection(const std::string& name);
  TomlSection* GetSectionOrCreate(const std::string& name);
  const TomlSection* GetSection(const std::string& name) const;
  void SetSection(const std::string& name, TomlSection section);
  void DeleteSection(const std::string& name);

  auto begin() { return sections_.begin(); }
  auto end() { return sections_.end(); }
  auto begin() const { return sections_.begin(); }
  auto end() const { return sections_.end(); }
  auto cbegin() const { return sections_.cbegin(); }
  auto cend() const { return sections_.cend(); }

 private:
  std::unordered_map<std::string, TomlSection> sections_;
};
}  // namespace cru::toml
