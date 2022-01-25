#include "cru/ui/UiManager.hpp"

#include "Helper.hpp"
#include "cru/common/io/FileStream.hpp"
#include "cru/common/io/OpenFileFlag.hpp"
#include "cru/common/io/Resource.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/Font.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "cru/ui/mapper/MapperRegistry.hpp"
#include "cru/ui/render/ScrollBar.hpp"
#include "cru/ui/style/ApplyBorderStyleInfo.hpp"
#include "cru/ui/style/Condition.hpp"
#include "cru/ui/style/Styler.hpp"
#include "cru/xml/XmlNode.hpp"
#include "cru/xml/XmlParser.hpp"

#include <optional>

namespace cru::ui {
using namespace cru::platform::graphics;
using namespace cru::ui::style;
using namespace cru::ui::helper;

UiManager* UiManager::GetInstance() {
  static UiManager* instance = new UiManager();
  GetUiApplication()->AddOnQuitHandler([] {
    delete instance;
    instance = nullptr;
  });
  return instance;
}

UiManager::UiManager() {
  const auto factory = GetGraphicsFactory();

  std::filesystem::path resourses_file =
      cru::io::GetResourceDir() / "cru/ui/DefaultResources.xml";

  if (!std::filesystem::exists(resourses_file)) {
    throw Exception(u"Default resources file not found.");
  }

  ReadResourcesFile(String::FromStdPath(resourses_file));

  theme_resource_.default_font_family = u"";

  theme_resource_.default_font =
      factory->CreateFont(theme_resource_.default_font_family, 24.0f);

  const auto black_brush =
      std::shared_ptr<platform::graphics::ISolidColorBrush>(
          factory->CreateSolidColorBrush(colors::black));
  theme_resource_.text_brush = black_brush;
  theme_resource_.text_selection_brush =
      factory->CreateSolidColorBrush(colors::skyblue);
  theme_resource_.caret_brush = black_brush;

  theme_resource_.menu_item_style.AddStyleRule(
      {NoCondition::Create(),
       BorderStyler::Create(
           ApplyBorderStyleInfo{std::nullopt, Thickness{0}, CornerRadius{0}}),
       u"DefaultMenuItem"});
}

UiManager::~UiManager() = default;

void UiManager::ReadResourcesFile(const String& file_path) {
  io::FileStream stream(file_path, io::OpenFileFlags::Read);
  auto xml_string = stream.ReadAllAsString();
  auto parser = xml::XmlParser(xml_string);
  auto xml_root = parser.Parse();

  for (auto child : xml_root->GetChildren()) {
    if (child->GetType() == xml::XmlNode::Type::Element) {
      auto c = child->AsElement();
      if (c->GetTag().CaseInsensitiveEqual(u"Resource")) {
        auto key = c->GetAttributeCaseInsensitive(u"key");
        if (key.CaseInsensitiveEqual(u"button-style")) {
          auto style_rule_set_mapper = mapper::MapperRegistry::GetInstance()
                                           ->GetRefMapper<StyleRuleSet>();
          auto style_rule_set =
              style_rule_set_mapper->MapFromXml(c->GetFirstChildElement());
          theme_resource_.button_style = style_rule_set;
        } else if (key.CaseInsensitiveEqual(u"text-box-style")) {
          auto style_rule_set_mapper = mapper::MapperRegistry::GetInstance()
                                           ->GetRefMapper<StyleRuleSet>();
          auto style_rule_set =
              style_rule_set_mapper->MapFromXml(c->GetFirstChildElement());
          theme_resource_.text_box_style = style_rule_set;
        }
      }
    }
  }
}
}  // namespace cru::ui
