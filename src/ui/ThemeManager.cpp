#include "cru/ui/ThemeManager.hpp"

#include "Helper.hpp"
#include "cru/common/StringUtil.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/gui/UiApplication.hpp"

namespace cru::ui {
ThemeManager* ThemeManager::GetInstance() {
  static ThemeManager instance;
  return &instance;
}

ThemeManager::ThemeManager() { Init(); }

void ThemeManager::Init() {
  theme_tree_.put("scrollbar.collapse-thumb.color",
                  colors::gray.WithAlpha(128).ToUtf8String());
  theme_tree_.put("scrollbar.arrow.normal.color", "black");
  theme_tree_.put("scrollbar.arrow.hover.color", "black");
  theme_tree_.put("scrollbar.arrow.press.color", "black");
  theme_tree_.put("scrollbar.arrow.disable.color", "black");
  theme_tree_.put("scrollbar.arrow-background.normal.color", "seashell");
  theme_tree_.put("scrollbar.arrow-background.hover.color", "seashell");
  theme_tree_.put("scrollbar.arrow-background.press.color", "seashell");
  theme_tree_.put("scrollbar.arrow-background.disable.color", "seashell");
  theme_tree_.put("scrollbar.slot.normal.color", "seashell");
  theme_tree_.put("scrollbar.slot.hover.color", "seashell");
  theme_tree_.put("scrollbar.slot.press.color", "seashell");
  theme_tree_.put("scrollbar.slot.disable.color", "seashell");
  theme_tree_.put("scrollbar.thumb.normal.color", "gray");
  theme_tree_.put("scrollbar.thumb.hover.color", "gray");
  theme_tree_.put("scrollbar.thumb.press.color", "gray");
  theme_tree_.put("scrollbar.thumb.disable.color", "gray");
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ThemeManager::GetBrush(std::u16string key) {
  std::u16string k = ToLower(key);
  auto cached_brush_iter = brushes_.find(k);
  if (cached_brush_iter != brushes_.cend()) {
    return cached_brush_iter->second;
  }

  auto color_string = ToUtf16(theme_tree_.get_value<std::string>(ToUtf8(key)));
  auto color = Color::Parse(color_string);
  if (!color) throw BadThemeResourceException("Value is not a valid color.");
  std::shared_ptr<platform::graphics::IBrush> brush =
      GetUiApplication()->GetGraphFactory()->CreateSolidColorBrush(*color);
  brushes_[k] = brush;
  return brush;
}

}  // namespace cru::ui
