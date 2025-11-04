#pragma once
#include "Base.h"

#include "cru/base/Base.h"
#include "cru/base/xml/XmlNode.h"
#include "mapper/MapperRegistry.h"
#include "style/StyleRuleSet.h"

#include <any>
#include <filesystem>
#include <format>
#include <typeindex>
#include <typeinfo>

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
  CRU_DEFINE_CLASS_LOG_TAG("ThemeResources");

 public:
  static std::unique_ptr<ThemeResourceDictionary> FromFile(
      std::filesystem::path file_path);

  explicit ThemeResourceDictionary(xml::XmlElementNode* xml_root,
                                   bool clone = true);
  ~ThemeResourceDictionary() override;

 public:
  template <typename T>
  T GetResource(std::string_view key) {
    auto find_result = resource_map_.find(std::string(key));
    if (find_result == resource_map_.cend()) {
      throw ThemeResourceKeyNotExistException(
          std::format("Theme resource key {} not exist.", key));
    }

    auto& cache = find_result->second.cache;
    auto cache_find_result = cache.find(typeid(T));
    if (cache_find_result != cache.cend()) {
      return std::any_cast<T>(cache_find_result->second);
    }

    auto mapper_registry = mapper::MapperRegistry::GetInstance();
    auto mapper = mapper_registry->GetMapper<T>();
    auto resource = mapper->MapFromXml(find_result->second.xml_node);
    cache[typeid(T)] = resource;
    return resource;
  }

 private:
  void UpdateResourceMap(xml::XmlElementNode* root_xml);

 private:
  struct ResourceEntry {
    CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ResourceEntry)
    CRU_DEFAULT_COPY(ResourceEntry)
    CRU_DEFAULT_MOVE(ResourceEntry)

    std::string name;
    xml::XmlElementNode* xml_node;
    std::unordered_map<std::type_index, std::any> cache;
  };

  std::unique_ptr<xml::XmlElementNode> xml_root_;
  std::unordered_map<std::string, ResourceEntry> resource_map_;
};
}  // namespace cru::ui
