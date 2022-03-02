#include "cru/ui/ThemeManager.h"

#include "Helper.h"
#include "cru/common/StringUtil.h"
#include "cru/common/io/FileStream.h"
#include "cru/common/io/Resource.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/ThemeResourceDictionary.h"
#include "cru/ui/style/StyleRuleSet.h"
#include "cru/xml/XmlParser.h"

namespace cru::ui {
ThemeManager* ThemeManager::GetInstance() {
  static ThemeManager instance;
  return &instance;
}

ThemeManager::ThemeManager() {
  std::filesystem::path resourses_file =
      cru::io::GetResourceDir() / "cru/ui/DefaultResources.xml";

  if (!std::filesystem::exists(resourses_file)) {
    throw Exception(u"Default resources file not found.");
  }

  PrependThemeResourceDictionary(
      ThemeResourceDictionary::FromFile(String::FromStdPath(resourses_file)));
}

ThemeManager::~ThemeManager() {}

std::vector<ThemeResourceDictionary*>
ThemeManager::GetThemeResourceDictionaryList() const {
  std::vector<ThemeResourceDictionary*> result;
  for (const auto& theme_resource_dictionary :
       theme_resource_dictionary_list_) {
    result.push_back(theme_resource_dictionary.get());
  }
  return result;
}

void ThemeManager::PrependThemeResourceDictionary(
    std::unique_ptr<ThemeResourceDictionary> theme_resource_dictionary) {
  theme_resource_dictionary_list_.insert(
      theme_resource_dictionary_list_.begin(),
      std::move(theme_resource_dictionary));
  theme_resource_change_event_.Raise(nullptr);
}

String ThemeManager::GetResourceString(const String& key) {
  return GetResource<String>(key);
}

std::shared_ptr<platform::graphics::IBrush> ThemeManager::GetResourceBrush(
    const String& key) {
  return GetResource<std::shared_ptr<platform::graphics::IBrush>>(key);
}

std::shared_ptr<platform::graphics::IFont> ThemeManager::GetResourceFont(
    const String& key) {
  return GetResource<std::shared_ptr<platform::graphics::IFont>>(key);
}

std::shared_ptr<style::StyleRuleSet> ThemeManager::GetResourceStyleRuleSet(
    const String& key) {
  return GetResource<std::shared_ptr<style::StyleRuleSet>>(key);
}
}  // namespace cru::ui
