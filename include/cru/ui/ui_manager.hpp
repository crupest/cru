#pragma once
#include "base.hpp"

#include "controls/base.hpp"

namespace cru::ui {
struct ThemeResources {
  std::shared_ptr<platform::graph::IFont> default_font;
  std::shared_ptr<platform::graph::IBrush> text_brush;
  std::shared_ptr<platform::graph::IBrush> text_selection_brush;
  std::shared_ptr<platform::graph::IBrush> caret_brush;
  controls::ButtonStyle button_style;
  controls::TextBoxBorderStyle text_box_border_style;
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
