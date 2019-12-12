#pragma once
#include "base.hpp"

#include "controls/button.hpp"

#include <memory>

namespace cru::platform::graph {
struct IBrush;
struct IFont;
}  // namespace cru::platform::graph

namespace cru::ui {
// TODO: Make this theme resource.
class PredefineResources : public Object {
 public:
  PredefineResources();
  PredefineResources(const PredefineResources& other) = delete;
  PredefineResources(PredefineResources&& other) = delete;
  PredefineResources& operator=(const PredefineResources& other) = delete;
  PredefineResources& operator=(PredefineResources&& other) = delete;
  ~PredefineResources() override = default;

  // region Button
  std::shared_ptr<platform::graph::IBrush> button_normal_border_brush;

  // region TextBlock
  std::shared_ptr<platform::graph::IBrush> text_block_selection_brush;
  std::shared_ptr<platform::graph::IBrush> text_block_text_brush;
  std::shared_ptr<platform::graph::IFont> text_block_font;
};

struct ThemeResources {
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

  const PredefineResources* GetPredefineResources() const {
    return predefine_resources_.get();
  }

  ThemeResources* GetThemeResources() { return &theme_resource_; }

 private:
  std::unique_ptr<PredefineResources> predefine_resources_;

  ThemeResources theme_resource_;
};
}  // namespace cru::ui
