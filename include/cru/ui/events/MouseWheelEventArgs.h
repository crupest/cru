#pragma once
#include "MouseEventArgs.h"

#include <cru/platform/gui/Input.h>

namespace cru::ui::events {
class CRU_UI_API MouseWheelEventArgs : public MouseEventArgs {
 public:
  MouseWheelEventArgs(Object* sender, Object* original_sender,
                      const Point& point, const float delta,
                      platform::gui::KeyModifier key_modifier)
      : MouseEventArgs(sender, original_sender, point),
        delta_(delta),
        key_modifier_(key_modifier) {}

  // Positive means down; Negative means up.
  float GetDelta() const { return delta_; }
  platform::gui::KeyModifier GetKeyModifier() const { return key_modifier_; }

 private:
  float delta_;
  platform::gui::KeyModifier key_modifier_;
};
}  // namespace cru::ui::events
