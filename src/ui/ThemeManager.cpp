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
  theme_tree_.put("scrollbar.arrow.normal.color", "#505050");
  theme_tree_.put("scrollbar.arrow.hover.color", "#505050");
  theme_tree_.put("scrollbar.arrow.press.color", "#ffffff");
  theme_tree_.put("scrollbar.arrow.disable.color", "#a3a3a3");
  theme_tree_.put("scrollbar.arrow-background.normal.color", "#f1f1f1");
  theme_tree_.put("scrollbar.arrow-background.hover.color", "#d2d2d2");
  theme_tree_.put("scrollbar.arrow-background.press.color", "#787878");
  theme_tree_.put("scrollbar.arrow-background.disable.color", "#f1f1f1");
  theme_tree_.put("scrollbar.slot.normal.color", "#f1f1f1");
  theme_tree_.put("scrollbar.slot.hover.color", "#f1f1f1");
  theme_tree_.put("scrollbar.slot.press.color", "#f1f1f1");
  theme_tree_.put("scrollbar.slot.disable.color", "#f1f1f1");
  theme_tree_.put("scrollbar.thumb.normal.color", "#c1c1c1");
  theme_tree_.put("scrollbar.thumb.hover.color", "#a8a8a8");
  theme_tree_.put("scrollbar.thumb.press.color", "#787878");
  theme_tree_.put("scrollbar.thumb.disable.color", "#c1c1c1");
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ThemeManager::GetBrush(std::u16string key) {
  std::u16string k = ToLower(key);
  auto cached_brush_iter = brushes_.find(k);
  if (cached_brush_iter != brushes_.cend()) {
    return cached_brush_iter->second;
  }

  auto color_string = ToUtf16(theme_tree_.get<std::string>(ToUtf8(key)));
  auto color = Color::Parse(color_string);
  if (!color) throw BadThemeResourceException("Value is not a valid color.");
  std::shared_ptr<platform::graphics::IBrush> brush =
      GetUiApplication()->GetGraphFactory()->CreateSolidColorBrush(*color);
  brushes_[k] = brush;
  return brush;
}

}  // namespace cru::ui
