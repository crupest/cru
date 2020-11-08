#pragma once
#include "../Base.hpp"

namespace cru::ui::controls {
using ButtonStateStyle = ui::BorderStyle;

struct ButtonStyle {
  // corresponds to ClickState::None
  ButtonStateStyle normal;
  // corresponds to ClickState::Hover
  ButtonStateStyle hover;
  // corresponds to ClickState::Press
  ButtonStateStyle press;
  // corresponds to ClickState::PressInactive
  ButtonStateStyle press_cancel;
};

struct TextBoxBorderStyle {
  ui::BorderStyle normal;
  ui::BorderStyle hover;
  ui::BorderStyle focus;
  ui::BorderStyle focus_hover;
};
}  // namespace cru::ui::controls
