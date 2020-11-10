#pragma once
#include "cru/ui/Base.hpp"

#include <optional>

namespace cru::ui::helper {
struct BorderStyleOfClickState {
  BorderStyleOfClickState(std::optional<BorderStyle> focus = std::nullopt,
                          std::optional<BorderStyle> not_focus = std::nullopt)
      : focus(std::move(focus)), not_focus(std::move(not_focus)) {}

  std::optional<BorderStyle> focus;
  std::optional<BorderStyle> not_focus;
};

struct BorderStyleList {
  BorderStyle default_one;
  std::optional<BorderStyleOfClickState> normal;
  std::optional<BorderStyleOfClickState> hover;
  std::optional<BorderStyleOfClickState> press;
  std::optional<BorderStyleOfClickState> press_inactive;
};
}  // namespace cru::ui::helper
