#pragma once
#include "Base.h"

#include <cru/base/Bitmask.h>

#include <string>

namespace cru::platform::gui {
namespace details {
struct TagMouseButton {};
}  // namespace details

using MouseButton = Bitmask<details::TagMouseButton>;
struct MouseButtons {
  constexpr static MouseButton None = MouseButton::FromOffset(0);
  constexpr static MouseButton Left = MouseButton::FromOffset(1);
  constexpr static MouseButton Middle = MouseButton::FromOffset(2);
  constexpr static MouseButton Right = MouseButton::FromOffset(3);
};

// Because of the complexity of keyboard layout, I only add code in US keyboard
// layout, the most widely used layout in China. We should try to make it easy
// to add new keyboard layout.
enum class KeyCode {
  Unknown,
  LeftButton,
  MiddleButton,
  RightButton,
  Escape,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  N0,
  N1,
  N2,
  N3,
  N4,
  N5,
  N6,
  N7,
  N8,
  N9,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  GraveAccent,
  Tab,
  CapsLock,
  LeftShift,
  LeftCtrl,
  LeftSuper,
  LeftAlt,
  Minus,
  Equal,
  Backspace,
  LeftSquareBracket,
  RightSquareBracket,
  BackSlash,
  Semicolon,
  Quote,
  Comma,
  Period,
  Slash,
  RightShift,
  RightCtrl,
  RightSuper,
  RightAlt,
  Insert,
  Delete,
  Home,
  End,
  PageUp,
  PageDown,
  Up,
  Left,
  Down,
  Right,
  PrintScreen,
  ScrollLock,
  Pause,
  NumPad0,
  NumPad1,
  NumPad2,
  NumPad3,
  NumPad4,
  NumPad5,
  NumPad6,
  NumPad7,
  NumPad8,
  NumPad9,
  LeftCommand,
  RightCommand,
  Return,
  Space
};

namespace details {
struct TagKeyModifier {};
}  // namespace details

using KeyModifier = Bitmask<details::TagKeyModifier>;

struct KeyModifiers {
  static constexpr KeyModifier None = KeyModifier::FromOffset(0);
  static constexpr KeyModifier Shift = KeyModifier::FromOffset(1);
  static constexpr KeyModifier Ctrl = KeyModifier::FromOffset(2);
  static constexpr KeyModifier Alt = KeyModifier::FromOffset(3);
  static constexpr KeyModifier Command = KeyModifier::FromOffset(4);
};

#ifdef CRU_PLATFORM_OSX
constexpr KeyModifier kKeyModifierCommand = KeyModifiers::Command;
#else
constexpr KeyModifier kKeyModifierCommand = KeyModifiers::Ctrl;
#endif

CRU_PLATFORM_GUI_API std::string ToString(KeyCode key_code);
CRU_PLATFORM_GUI_API std::string ToString(KeyModifier key_modifier,
                                          std::string_view separator = "+");
}  // namespace cru::platform::gui
