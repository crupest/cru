#include "cru/ui/UiManager.hpp"

#include "Helper.hpp"
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
}

UiManager::~UiManager() = default;

}  // namespace cru::ui
