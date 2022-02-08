#include "cru/platform/gui/Keyboard.h"

#include <array>
#include <string>
#include <string_view>

namespace cru::platform::gui {
const std::array<StringView, static_cast<int>(KeyCode::NumPad9) + 1>
    key_code_string_list{u"Unknown",
                         u"LeftButton",
                         u"MiddleButton",
                         u"RightButton",
                         u"Escape",
                         u"F1",
                         u"F2",
                         u"F3",
                         u"F4",
                         u"F5",
                         u"F6",
                         u"F7",
                         u"F8",
                         u"F9",
                         u"F10",
                         u"F11",
                         u"F12",
                         u"N0",
                         u"N1",
                         u"N2",
                         u"N3",
                         u"N4",
                         u"N5",
                         u"N6",
                         u"N7",
                         u"N8",
                         u"N9",
                         u"A",
                         u"B",
                         u"C",
                         u"D",
                         u"E",
                         u"F",
                         u"G",
                         u"H",
                         u"I",
                         u"J",
                         u"K",
                         u"L",
                         u"M",
                         u"N",
                         u"O",
                         u"P",
                         u"Q",
                         u"R",
                         u"S",
                         u"T",
                         u"U",
                         u"V",
                         u"W",
                         u"X",
                         u"Y",
                         u"Z",
                         u"GraveAccent",
                         u"Tab",
                         u"CapsLock",
                         u"LeftShift",
                         u"LeftCtrl",
                         u"LeftSuper",
                         u"LeftAlt",
                         u"Minus",
                         u"Equal",
                         u"Backspace",
                         u"LeftSquareBracket",
                         u"RightSquareBracket",
                         u"BackSlash",
                         u"Semicolon",
                         u"Quote",
                         u"Comma",
                         u"Period",
                         u"Slash",
                         u"RightShift",
                         u"RightCtrl",
                         u"RightSuper",
                         u"RightAlt",
                         u"Insert",
                         u"Delete",
                         u"Home",
                         u"End",
                         u"PageUp",
                         u"PageDown",
                         u"Up",
                         u"Left",
                         u"Down",
                         u"Right",
                         u"PrintScreen",
                         u"ScrollLock",
                         u"Pause",
                         u"NumPad0",
                         u"NumPad1",
                         u"NumPad2",
                         u"NumPad3",
                         u"NumPad4",
                         u"NumPad5",
                         u"NumPad6",
                         u"NumPad7",
                         u"NumPad8",
                         u"NumPad9"};

String ToString(KeyCode key_code) {
  if (static_cast<int>(key_code) < 0 ||
      static_cast<int>(key_code) >=
          static_cast<int>(key_code_string_list.size()))
    return u"UNKNOWN_KEYCODENAME";

  return key_code_string_list[static_cast<int>(key_code)].ToString();
}

String ToString(KeyModifier key_modifier, StringView separator) {
  std::vector<String> list;
  if (key_modifier & KeyModifiers::shift) {
    list.push_back(u"Shift");
  }

  if (key_modifier & KeyModifiers::ctrl) {
    list.push_back(u"Ctrl");
  }

  if (key_modifier & KeyModifiers::alt) {
    list.push_back(u"Shift");
  }

  if (list.empty()) return u"";
  String result = list.front();
  for (auto iter = list.cbegin() + 1; iter != list.cend(); ++iter) {
    result += separator;
    result += *iter;
  }

  return result;
}
}  // namespace cru::platform::gui
