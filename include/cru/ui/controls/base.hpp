#pragma once
#include "../Base.hpp"

namespace cru::ui::controls {
using ButtonStateStyle = BorderStyle;

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
  BorderStyle normal;
  BorderStyle hover;
  BorderStyle focus;
  BorderStyle focus_hover;
};
}  // namespace cru::ui::controls
