#pragma once
#include "Base.hpp"

#include "controls/Base.hpp"
#include "cru/platform/graphics/Brush.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "style/StyleRuleSet.hpp"

#include <gsl/pointers>
#include <memory>
#include <string>
#include <unordered_map>

namespace cru::ui {
struct ThemeResources {
  String default_font_family;
  std::shared_ptr<platform::graphics::IFont> default_font;
  std::shared_ptr<platform::graphics::IBrush> text_brush;
  std::shared_ptr<platform::graphics::IBrush> text_selection_brush;
  std::shared_ptr<platform::graphics::IBrush> caret_brush;

  std::shared_ptr<style::StyleRuleSet> button_style;
  std::shared_ptr<style::StyleRuleSet> text_box_style;

  style::StyleRuleSet menu_item_style;
};

class CRU_UI_API UiManager : public Object {
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

  void ReadResourcesFile(const String& file_path);

 private:
  ThemeResources theme_resource_;
};
}  // namespace cru::ui
