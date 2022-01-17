#pragma once
#include "UiEventArgs.hpp"

#include "cru/platform/gui/Keyboard.hpp"

namespace cru::ui::events {
class CRU_UI_API KeyEventArgs : public UiEventArgs {
 public:
  KeyEventArgs(Object* sender, Object* original_sender,
               platform::gui::KeyCode key_code,
               platform::gui::KeyModifier key_modifier)
      : UiEventArgs(sender, original_sender),
        key_code_(key_code),
        key_modifier_(key_modifier) {}
  KeyEventArgs(const KeyEventArgs& other) = default;
  KeyEventArgs(KeyEventArgs&& other) = default;
  KeyEventArgs& operator=(const KeyEventArgs& other) = default;
  KeyEventArgs& operator=(KeyEventArgs&& other) = default;
  ~KeyEventArgs() override = default;

  platform::gui::KeyCode GetKeyCode() const { return key_code_; }
  platform::gui::KeyModifier GetKeyModifier() const { return key_modifier_; }

 private:
  platform::gui::KeyCode key_code_;
  platform::gui::KeyModifier key_modifier_;
};

}  // namespace cru::ui::event
