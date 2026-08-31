#pragma once
#include "Base.h"
#include "datamodel/Base.h"

#include <cru/base/Base.h>
#include <cru/base/Xml.h>

#include <any>
#include <filesystem>
#include <format>
#include <typeindex>
#include <variant>

namespace cru::ui {
class CRU_UI_API ThemeResourceKeyNotExistException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_UI_API BadThemeResourceException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_UI_API ThemeResourceDictionary : public Object {
 private:
  constexpr static auto kLogTag = "cru::ui::ThemeResources";

 public:
  static std::unique_ptr<ThemeResourceDictionary> FromFile(
      std::filesystem::path file_path);

  explicit ThemeResourceDictionary(xml::XmlElementNode* xml_root,
                                   bool clone = true);

 public:
  template <typename T>
  T GetResource(std::string_view key) {
    auto find_result = resource_map_.find(std::string(key));
    if (find_result == resource_map_.cend()) {
      throw ThemeResourceKeyNotExistException(
          std::format("Theme resource key {} not exist.", key));
    }
    auto& resource_entry = find_result->second;

    if (std::holds_alternative<std::string>(resource_entry.value)) {
      if constexpr (std::is_same_v<T, std::string>) {
        return std::get<std::string>(resource_entry.value);
      } else {
        throw Exception(
            "Resource is pure text and cannot convert to non std::string.");
      }
    }

    auto& cache = resource_entry.cache;
    auto cache_find_result = cache.find(typeid(T));
    if (cache_find_result != cache.cend()) {
      return std::any_cast<T>(cache_find_result->second);
    }

    auto* data_type = datamodel::GetUiDataTypeRegistry()->GetDataType<T>();
    if (!data_type) {
      throw BadThemeResourceException(std::format(
          "No data type registered for theme resource key {}.", key));
    }

    auto convert_result = data_type->ConvertFromXml(
        std::get<xml::XmlElementNode*>(resource_entry.value));
    if (!convert_result.IsSuccess()) {
      std::string errors;
      for (const auto& error : convert_result.GetErrors()) {
        if (!errors.empty()) {
          errors += "; ";
        }
        errors += error;
      }

      throw BadThemeResourceException(
          std::format("Failed to convert theme resource key {}: {}", key,
                      errors.empty() ? "unknown error" : errors));
    }

    auto resource = convert_result.GetValue();
    cache[typeid(T)] = resource;

    return resource;
  }

 private:
  struct ResourceEntry {
    std::string name;
    std::variant<xml::XmlElementNode*, std::string> value;
    std::unordered_map<std::type_index, std::any> cache;
  };

  void UpdateResourceMap(xml::XmlElementNode* root_xml);

 private:
  std::unique_ptr<xml::XmlElementNode> xml_root_;
  std::unordered_map<std::string, ResourceEntry> resource_map_;
};
}  // namespace cru::ui
