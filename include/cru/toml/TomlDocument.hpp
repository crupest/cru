#pragma once

#include "cru/common/Base.hpp"
#include "cru/common/String.hpp"

#include <optional>
#include <unordered_map>

namespace cru::toml {
class TomlSection {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(TomlSection)
  CRU_DEFAULT_COPY(TomlSection)
  CRU_DEFAULT_MOVE(TomlSection)

 public:
  std::optional<String> GetValue(const String& key) const;
  void SetValue(const String& key, String value);
  void DeleteValue(const String& key);

  auto begin() { return values_.begin(); }
  auto end() { return values_.end(); }
  auto begin() const { return values_.begin(); }
  auto end() const { return values_.end(); }
  auto cbegin() const { return values_.cbegin(); }
  auto cend() const { return values_.cend(); }

 private:
  std::unordered_map<String, String> values_;
};

class TomlDocument {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(TomlDocument)
  CRU_DEFAULT_COPY(TomlDocument)
  CRU_DEFAULT_MOVE(TomlDocument)

 public:
  TomlSection* GetSection(const String& name);
  const TomlSection* GetSection(const String& name) const;
  void SetSection(const String& name, TomlSection section);
  void DeleteSection(const String& name);

  auto begin() { return sections_.begin(); }
  auto end() { return sections_.end(); }
  auto begin() const { return sections_.begin(); }
  auto end() const { return sections_.end(); }
  auto cbegin() const { return sections_.cbegin(); }
  auto cend() const { return sections_.cend(); }

 private:
  std::unordered_map<String, TomlSection> sections_;
};
}  // namespace cru::toml
