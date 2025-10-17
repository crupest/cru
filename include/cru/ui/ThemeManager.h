#pragma once
#include "Base.h"
#include "cru/base/Event.h"
#include "cru/base/Format.h"
#include "cru/ui/ThemeResourceDictionary.h"

#include <vector>

namespace cru::ui {

class CRU_UI_API ThemeManager : public Object {
 public:
  static ThemeManager* GetInstance();

 private:
  ThemeManager();

 public:
  CRU_DELETE_COPY(ThemeManager)
  CRU_DELETE_MOVE(ThemeManager)

  ~ThemeManager() override;

  std::vector<ThemeResourceDictionary*> GetThemeResourceDictionaryList() const;

  void PrependThemeResourceDictionary(
      std::unique_ptr<ThemeResourceDictionary> theme_resource_dictionary);

  template <typename T>
  T GetResource(std::string_view key) {
    for (const auto& resource_dictionary : theme_resource_dictionary_list_) {
      try {
        return resource_dictionary->GetResource<T>(key);
      } catch (ThemeResourceKeyNotExistException&) {
      }
    }
    throw ThemeResourceKeyNotExistException(
        std::format("Theme resource key {} not exist.", key));
  }

  std::string GetResourceString(std::string_view key);

  std::shared_ptr<platform::graphics::IBrush> GetResourceBrush(
      std::string_view key);

  std::shared_ptr<platform::graphics::IFont> GetResourceFont(
      std::string_view key);

  std::shared_ptr<style::StyleRuleSet> GetResourceStyleRuleSet(
      std::string_view key);

  IEvent<std::nullptr_t>* ThemeResourceChangeEvent() {
    return &theme_resource_change_event_;
  }

 private:
  Event<std::nullptr_t> theme_resource_change_event_;
  std::vector<std::unique_ptr<ThemeResourceDictionary>>
      theme_resource_dictionary_list_;
};
}  // namespace cru::ui
