#include "cru/ui/ThemeManager.hpp"
#include <memory>

#include "Helper.hpp"
#include "cru/common/StringUtil.hpp"
#include "cru/common/io/FileStream.hpp"
#include "cru/common/io/Resource.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/style/StyleRuleSet.hpp"
#include "cru/xml/XmlParser.hpp"

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

std::shared_ptr<style::StyleRuleSet> ThemeManager::GetResourceStyleRuleSet(
    const String& key) {
  return GetResource<std::shared_ptr<style::StyleRuleSet>>(key);
}

void ThemeManager::ReadResourcesFile(const String& file_path) {
  io::FileStream stream(file_path, io::OpenFileFlags::Read);
  auto xml_string = stream.ReadAllAsString();
  auto parser = xml::XmlParser(xml_string);
  theme_resource_xml_root_.reset(parser.Parse());
  theme_resource_map_.clear();

  if (!theme_resource_xml_root_->GetTag().CaseInsensitiveEqual(u"Theme")) {
    throw Exception(u"Root tag of theme resource file must be \"Theme\".");
  }

  for (auto child : theme_resource_xml_root_->GetChildren()) {
    if (child->IsElementNode()) {
      auto c = child->AsElement();
      if (c->GetTag().CaseInsensitiveEqual(u"Resource")) {
        auto key_attr = c->GetOptionalAttributeCaseInsensitive(u"key");
        if (!key_attr) {
          throw Exception(u"\"key\" attribute is required for resource.");
        }
        theme_resource_map_[*key_attr] = c->GetFirstChildElement();
      }
    }
  }
}

}  // namespace cru::ui
