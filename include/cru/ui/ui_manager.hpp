#pragma once
#include "base.hpp"

#include "controls/button.hpp"

#include <memory>

namespace cru::platform::graph {
struct IBrush;
struct IFont;
}  // namespace cru::platform::graph

namespace cru::ui {
struct ThemeResources {
  std::shared_ptr<platform::graph::IFont> default_font;
  std::shared_ptr<platform::graph::IBrush> text_brush;
  std::shared_ptr<platform::graph::IBrush> text_selection_brush;
  std::shared_ptr<platform::graph::IBrush> caret_brush;
  controls::ButtonStyle button_style;
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
  ~UiManager() override = default;

  ThemeResources* GetThemeResources() { return &theme_resource_; }

 private:
  ThemeResources theme_resource_;
};
}  // namespace cru::ui
