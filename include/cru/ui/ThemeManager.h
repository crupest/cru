#pragma once
#include "Base.h"
#include "cru/common/Base.h"
#include "cru/common/Event.h"
#include "cru/common/Exception.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/ui/mapper/MapperRegistry.h"
#include "cru/ui/style/StyleRuleSet.h"
#include "cru/xml/XmlNode.h"

#include <any>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace cru::ui {
class CRU_UI_API ThemeResourceKeyNotExistException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_UI_API BadThemeResourceException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_UI_API ThemeManager : public Object {
 public:
  static ThemeManager* GetInstance();

 private:
  ThemeManager();

 public:
  CRU_DELETE_COPY(ThemeManager)
  CRU_DELETE_MOVE(ThemeManager)

  ~ThemeManager() override;

  IEvent<std::nullptr_t>* ThemeResourceChangeEvent() {
    return &theme_resource_change_event_;
  }

  void ReadResourcesFile(const String& file_path);

  void SetThemeXml(xml::XmlElementNode* root);

  template <typename T>
  T GetResource(const String& key) {
    auto find_result = theme_resource_map_.find(key);
    if (find_result == theme_resource_map_.cend()) {
      throw ThemeResourceKeyNotExistException(
          Format(u"Theme resource key \"%s\" not exist.", key));
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

  std::shared_ptr<platform::graphics::IBrush> GetResourceBrush(
      const String& key);

  std::shared_ptr<platform::graphics::IFont> GetResourceFont(const String& key);

  std::shared_ptr<style::StyleRuleSet> GetResourceStyleRuleSet(
      const String& key);

 private:
  struct ResourceEntry {
    CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(ResourceEntry)
    CRU_DEFAULT_COPY(ResourceEntry)
    CRU_DEFAULT_MOVE(ResourceEntry)

    String name;
    xml::XmlElementNode* xml_node;
    std::unordered_map<std::type_index, std::any> cache;
  };

  Event<std::nullptr_t> theme_resource_change_event_;
  std::unique_ptr<xml::XmlElementNode> theme_resource_xml_root_;
  std::unordered_map<String, ResourceEntry> theme_resource_map_;
};
}  // namespace cru::ui
