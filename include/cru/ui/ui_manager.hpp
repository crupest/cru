#pragma once
#include "base.hpp"
#include "cru/common/base.hpp"

#include <memory>

namespace cru::platform::graph {
class Brush;
class Font;
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
  std::shared_ptr<platform::graph::Brush> button_normal_border_brush;

  // region TextBlock
  std::shared_ptr<platform::graph::Brush> text_block_selection_brush;
  std::shared_ptr<platform::graph::Brush> text_block_text_brush;
  std::shared_ptr<platform::graph::Font> text_block_font;
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

 private:
  std::unique_ptr<PredefineResources> predefine_resources_;
};
}  // namespace cru::ui