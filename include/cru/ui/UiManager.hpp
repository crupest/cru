#pragma once
#include "Base.hpp"

#include "controls/Base.hpp"
#include "style/StyleRuleSet.hpp"

#include <memory>
#include <string>

namespace cru::ui {
struct ThemeResources {
  std::u16string default_font_family;
  std::shared_ptr<platform::graphics::IFont> default_font;
  std::shared_ptr<platform::graphics::IBrush> text_brush;
  std::shared_ptr<platform::graphics::IBrush> text_selection_brush;
  std::shared_ptr<platform::graphics::IBrush> caret_brush;
  style::StyleRuleSet button_style;
  style::StyleRuleSet text_box_style;
};

class UiManager : public Object {
 public:
  static UiManager* GetInstance();

 private:
  UiManager();

 public:
  UiManager(const UiManager& other) = delete;
  UiManager(UiManager&& other) = delete;
  UiManager& operator=(const UiManager& other) = delete;
  UiManager& operator=(UiManager&& other) = delete;
  ~UiManager() override;

  ThemeResources* GetThemeResources() { return &theme_resource_; }

 private:
  ThemeResources theme_resource_;
};
}  // namespace cru::ui
