#include "cru/ui/ThemeManager.h"

#include "Helper.h"
#include "cru/common/StringUtil.h"
#include "cru/common/io/FileStream.h"
#include "cru/common/io/Resource.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"
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

  ReadResourcesFile(String::FromStdPath(resourses_file));
}

ThemeManager::~ThemeManager() {}

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

void ThemeManager::ReadResourcesFile(const String& file_path) {
  io::FileStream stream(file_path, io::OpenFileFlags::Read);
  auto xml_string = stream.ReadAllAsString();
  auto parser = xml::XmlParser(xml_string);
  SetThemeXml(parser.Parse());
}

void ThemeManager::SetThemeXml(xml::XmlElementNode* root) {
  theme_resource_xml_root_.reset(root);
  theme_resource_map_.clear();

  if (!theme_resource_xml_root_->GetTag().CaseInsensitiveEqual(u"Theme")) {
    throw Exception(u"Root tag of theme must be \"Theme\".");
  }

  for (auto child : theme_resource_xml_root_->GetChildren()) {
    if (child->IsElementNode()) {
      auto c = child->AsElement();
      if (c->GetTag().CaseInsensitiveEqual(u"Resource")) {
        auto key_attr = c->GetOptionalAttributeCaseInsensitive(u"key");
        if (!key_attr) {
          throw Exception(u"\"key\" attribute is required for resource.");
        }
        if (!c->GetChildElementCount()) {
          throw Exception(u"Resource must have only one child element.");
        }

        ResourceEntry entry;

        entry.name = *key_attr;
        entry.xml_node = c->GetFirstChildElement();

        theme_resource_map_[entry.name] = std::move(entry);
      }
    }
  }

  theme_resource_change_event_.Raise(nullptr);
}
}  // namespace cru::ui
