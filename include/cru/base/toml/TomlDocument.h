#pragma once

#include "../Base.h"
#include "../Json.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cru::toml {
class CRU_BASE_API TomlSection {
 public:
  using value_type = std::pair<std::string, json::JsonValue>;
  using storage_type = std::vector<value_type>;
  using size_type = typename storage_type::size_type;
  using difference_type = typename storage_type::difference_type;
  using reference = typename storage_type::reference;
  using const_reference = typename storage_type::const_reference;
  using pointer = typename storage_type::pointer;
  using const_pointer = typename storage_type::const_pointer;
  using iterator = typename storage_type::iterator;
  using const_iterator = typename storage_type::const_iterator;

  bool HasKey(std::string_view key) const;
  json::JsonValue& GetValue(std::string_view key);
  const json::JsonValue& GetValue(std::string_view key) const;
  void SetValue(std::string_view key, json::JsonValue value);
  bool DeleteValue(std::string_view key);

  iterator begin() { return values_.begin(); }
  iterator end() { return values_.end(); }
  const_iterator begin() const { return values_.begin(); }
  const_iterator end() const { return values_.end(); }
  const_iterator cbegin() const { return values_.cbegin(); }
  const_iterator cend() const { return values_.cend(); }

 private:
  storage_type values_;
};

class CRU_BASE_API TomlDocument {
 public:
  using value_type = std::pair<std::string, TomlSection>;
  using storage_type = std::vector<value_type>;
  using size_type = typename storage_type::size_type;
  using difference_type = typename storage_type::difference_type;
  using reference = typename storage_type::reference;
  using const_reference = typename storage_type::const_reference;
  using pointer = typename storage_type::pointer;
  using const_pointer = typename storage_type::const_pointer;
  using iterator = typename storage_type::iterator;
  using const_iterator = typename storage_type::const_iterator;

  bool HasSection(std::string_view name) const;
  TomlSection& GetSection(std::string_view name);
  const TomlSection& GetSection(std::string_view name) const;
  TomlSection& GetSectionOrCreate(std::string_view name);
  void SetSection(std::string_view name, TomlSection section);
  bool DeleteSection(std::string_view name);

  iterator begin() { return sections_.begin(); }
  iterator end() { return sections_.end(); }
  const_iterator begin() const { return sections_.begin(); }
  const_iterator end() const { return sections_.end(); }
  const_iterator cbegin() const { return sections_.cbegin(); }
  const_iterator cend() const { return sections_.cend(); }

 private:
  storage_type sections_;
};
}  // namespace cru::toml
