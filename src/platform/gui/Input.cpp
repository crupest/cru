#include "cru/platform/gui/Input.h"

#include <array>
#include <string>
#include <string_view>

namespace cru::platform::gui {
const std::array<std::string_view, static_cast<int>(KeyCode::NumPad9) + 1>
    key_code_string_list{"Unknown",
                         "LeftButton",
                         "MiddleButton",
                         "RightButton",
                         "Escape",
                         "F1",
                         "F2",
                         "F3",
                         "F4",
                         "F5",
                         "F6",
                         "F7",
                         "F8",
                         "F9",
                         "F10",
                         "F11",
                         "F12",
                         "N0",
                         "N1",
                         "N2",
                         "N3",
                         "N4",
                         "N5",
                         "N6",
                         "N7",
                         "N8",
                         "N9",
                         "A",
                         "B",
                         "C",
                         "D",
                         "E",
                         "F",
                         "G",
                         "H",
                         "I",
                         "J",
                         "K",
                         "L",
                         "M",
                         "N",
                         "O",
                         "P",
                         "Q",
                         "R",
                         "S",
                         "T",
                         "U",
                         "V",
                         "W",
                         "X",
                         "Y",
                         "Z",
                         "Grave",
                         "Tab",
                         "CapsLock",
                         "LeftShift",
                         "LeftCtrl",
                         "LeftSuper",
                         "LeftAlt",
                         "Minus",
                         "Equal",
                         "Backspace",
                         "LeftSquareBracket",
                         "RightSquareBracket",
                         "BackSlash",
                         "Semicolon",
                         "Quote",
                         "Comma",
                         "Period",
                         "Slash",
                         "RightShift",
                         "RightCtrl",
                         "RightSuper",
                         "RightAlt",
                         "Insert",
                         "Delete",
                         "Home",
                         "End",
                         "PageUp",
                         "PageDown",
                         "Up",
                         "Left",
                         "Down",
                         "Right",
                         "PrintScreen",
                         "ScrollLock",
                         "Pause",
                         "NumPad0",
                         "NumPad1",
                         "NumPad2",
                         "NumPad3",
                         "NumPad4",
                         "NumPad5",
                         "NumPad6",
                         "NumPad7",
                         "NumPad8",
                         "NumPad9"};

std::string ToString(KeyCode key_code) {
  if (static_cast<int>(key_code) < 0 ||
      static_cast<int>(key_code) >=
          static_cast<int>(key_code_string_list.size()))
    return "UNKNOWN_KEYCODENAME";

  return std::string(key_code_string_list[static_cast<int>(key_code)]);
}

std::string ToString(KeyModifier key_modifier, std::string_view separator) {
  std::vector<std::string> list;
  if (key_modifier & KeyModifiers::Shift) {
    list.push_back("Shift");
  }

  if (key_modifier & KeyModifiers::Ctrl) {
    list.push_back("Ctrl");
  }

  if (key_modifier & KeyModifiers::Alt) {
    list.push_back("Shift");
  }

  if (list.empty()) return "";
  std::string result = list.front();
  for (auto iter = list.cbegin() + 1; iter != list.cend(); ++iter) {
    result += separator;
    result += *iter;
  }

  return result;
}
}  // namespace cru::platform::gui
