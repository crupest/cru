#pragma once
#include "MouseEventArgs.h"

#include "cru/platform/gui/Input.h"

namespace cru::ui::events {

class CRU_UI_API MouseButtonEventArgs : public MouseEventArgs {
 public:
  MouseButtonEventArgs(Object* sender, Object* original_sender,
                       const Point& point,
                       const platform::gui::MouseButton button,
                       platform::gui::KeyModifier key_modifier)
      : MouseEventArgs(sender, original_sender, point),
        button_(button),
        key_modifier_(key_modifier) {}
  MouseButtonEventArgs(Object* sender, Object* original_sender,
                       const platform::gui::MouseButton button,
                       platform::gui::KeyModifier key_modifier)
      : MouseEventArgs(sender, original_sender),
        button_(button),
        key_modifier_(key_modifier) {}
  MouseButtonEventArgs(const MouseButtonEventArgs& other) = default;
  MouseButtonEventArgs(MouseButtonEventArgs&& other) = default;
  MouseButtonEventArgs& operator=(const MouseButtonEventArgs& other) = default;
  MouseButtonEventArgs& operator=(MouseButtonEventArgs&& other) = default;
  ~MouseButtonEventArgs() override = default;

  platform::gui::MouseButton GetButton() const { return button_; }
  platform::gui::KeyModifier GetKeyModifier() const { return key_modifier_; }

 private:
  platform::gui::MouseButton button_;
  platform::gui::KeyModifier key_modifier_;
};
}  // namespace cru::ui::events
