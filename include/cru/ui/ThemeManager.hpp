#pragma once
#include "Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/common/Exception.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/style/StyleRuleSet.hpp"
#include "cru/xml/XmlNode.hpp"

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

  template <typename T>
  T GetResource(const String& key) {
    auto find_result = theme_resource_map_.find(key);
    if (find_result == theme_resource_map_.cend()) {
      throw ThemeResourceKeyNotExistException(
          Format(u"Theme resource key \"%s\" not exist.", key));
    }

    auto mapper_registry = mapper::MapperRegistry::GetInstance();
    auto mapper = mapper_registry->GetMapper<T>();
    return mapper->MapFromXml(find_result->second);
  }

  std::shared_ptr<platform::graphics::IBrush> GetResourceBrush(
      const String& key);

  std::shared_ptr<platform::graphics::IFont> GetResourceFont(const String& key);

  std::shared_ptr<style::StyleRuleSet> GetResourceStyleRuleSet(
      const String& key);

 private:
  Event<std::nullptr_t> theme_resource_change_event_;
  std::unique_ptr<xml::XmlElementNode> theme_resource_xml_root_;
  std::unordered_map<String, xml::XmlElementNode*> theme_resource_map_;
};
}  // namespace cru::ui
