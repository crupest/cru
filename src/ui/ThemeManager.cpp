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
  theme_resource_map_.emplace(u"scrollbar.collapse-thumb.color",
                              colors::gray.WithAlpha(128).ToString());
  theme_resource_map_.emplace(u"scrollbar.arrow.normal.color", u"#505050");
  theme_resource_map_.emplace(u"scrollbar.arrow.hover.color", u"#505050");
  theme_resource_map_.emplace(u"scrollbar.arrow.press.color", u"#ffffff");
  theme_resource_map_.emplace(u"scrollbar.arrow.disable.color", u"#a3a3a3");
  theme_resource_map_.emplace(u"scrollbar.arrow-background.normal.color",
                              u"#f1f1f1");
  theme_resource_map_.emplace(u"scrollbar.arrow-background.hover.color",
                              u"#d2d2d2");
  theme_resource_map_.emplace(u"scrollbar.arrow-background.press.color",
                              u"#787878");
  theme_resource_map_.emplace(u"scrollbar.arrow-background.disable.color",
                              u"#f1f1f1");
  theme_resource_map_.emplace(u"scrollbar.slot.normal.color", u"#f1f1f1");
  theme_resource_map_.emplace(u"scrollbar.slot.hover.color", u"#f1f1f1");
  theme_resource_map_.emplace(u"scrollbar.slot.press.color", u"#f1f1f1");
  theme_resource_map_.emplace(u"scrollbar.slot.disable.color", u"#f1f1f1");
  theme_resource_map_.emplace(u"scrollbar.thumb.normal.color", u"#c1c1c1");
  theme_resource_map_.emplace(u"scrollbar.thumb.hover.color", u"#a8a8a8");
  theme_resource_map_.emplace(u"scrollbar.thumb.press.color", u"#787878");
  theme_resource_map_.emplace(u"scrollbar.thumb.disable.color", u"#c1c1c1");
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ThemeManager::GetBrush(StringView key) {
  auto k = key.ToString();
  auto cached_brush_iter = brushes_.find(k);
  if (cached_brush_iter != brushes_.cend()) {
    return cached_brush_iter->second;
  }

  auto color_string_iter = theme_resource_map_.find(k);
  if (color_string_iter == theme_resource_map_.cend()) {
    throw ThemeResourceKeyNotExistException(u"Key do not exist.");
  }

  auto color = Color::Parse(color_string_iter->second);
  if (!color) throw BadThemeResourceException(u"Value is not a valid color.");
  std::shared_ptr<platform::graphics::IBrush> brush =
      GetUiApplication()->GetGraphicsFactory()->CreateSolidColorBrush(*color);
  brushes_[k] = brush;
  return brush;
}

}  // namespace cru::ui
