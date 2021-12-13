#pragma once
#include "MouseEventArgs.hpp"

#include "cru/platform/gui/Keyboard.hpp"

namespace cru::ui::events {

class MouseButtonEventArgs : public MouseEventArgs {
 public:
  MouseButtonEventArgs(Object* sender, Object* original_sender,
                       const Point& point, const MouseButton button,
                       platform::gui::KeyModifier key_modifier)
      : MouseEventArgs(sender, original_sender, point),
        button_(button),
        key_modifier_(key_modifier) {}
  MouseButtonEventArgs(Object* sender, Object* original_sender,
                       const MouseButton button,
                       platform::gui::KeyModifier key_modifier)
      : MouseEventArgs(sender, original_sender),
        button_(button),
        key_modifier_(key_modifier) {}
  MouseButtonEventArgs(const MouseButtonEventArgs& other) = default;
  MouseButtonEventArgs(MouseButtonEventArgs&& other) = default;
  MouseButtonEventArgs& operator=(const MouseButtonEventArgs& other) = default;
  MouseButtonEventArgs& operator=(MouseButtonEventArgs&& other) = default;
  ~MouseButtonEventArgs() override = default;

  MouseButton GetButton() const { return button_; }
  platform::gui::KeyModifier GetKeyModifier() const { return key_modifier_; }

 private:
  MouseButton button_;
  platform::gui::KeyModifier key_modifier_;
};
}  // namespace cru::ui::event
