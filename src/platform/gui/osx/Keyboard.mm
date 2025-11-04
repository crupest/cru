#include "cru/platform/gui/osx/Keyboard.h"
#import "KeyboardPrivate.h"

#import <AppKit/NSText.h>
#import <Carbon/Carbon.h>

namespace cru::platform::gui::osx {
KeyCode KeyCodeFromOsxToCru(unsigned short n) {
  switch (n) {
#define CRU_DEFINE_KEYCODE_MAP(osx, cru) \
  case osx:                              \
    return cru;
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_0, KeyCode::N0)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_1, KeyCode::N1)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_2, KeyCode::N2)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_3, KeyCode::N3)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_4, KeyCode::N4)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_5, KeyCode::N5)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_6, KeyCode::N6)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_7, KeyCode::N7)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_8, KeyCode::N8)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_9, KeyCode::N9)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_A, KeyCode::A)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_B, KeyCode::B)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_C, KeyCode::C)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_D, KeyCode::D)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_E, KeyCode::E)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_F, KeyCode::F)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_G, KeyCode::G)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_H, KeyCode::H)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_I, KeyCode::I)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_J, KeyCode::J)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_K, KeyCode::K)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_L, KeyCode::L)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_M, KeyCode::M)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_N, KeyCode::N)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_O, KeyCode::O)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_P, KeyCode::P)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Q, KeyCode::Q)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_R, KeyCode::R)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_S, KeyCode::S)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_T, KeyCode::T)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_U, KeyCode::U)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_V, KeyCode::V)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_W, KeyCode::W)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_X, KeyCode::X)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Y, KeyCode::Y)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Z, KeyCode::Z)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Comma, KeyCode::Comma)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Period, KeyCode::Period)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Slash, KeyCode::Slash)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Semicolon, KeyCode::Semicolon)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Quote, KeyCode::Quote)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_LeftBracket, KeyCode::LeftSquareBracket)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_RightBracket, KeyCode::RightSquareBracket)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Minus, KeyCode::Minus)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Equal, KeyCode::Equal)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Backslash, KeyCode::BackSlash)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Grave, KeyCode::GraveAccent)
    CRU_DEFINE_KEYCODE_MAP(kVK_Escape, KeyCode::Escape)
    CRU_DEFINE_KEYCODE_MAP(kVK_Tab, KeyCode::Tab)
    CRU_DEFINE_KEYCODE_MAP(kVK_CapsLock, KeyCode::CapsLock)
    CRU_DEFINE_KEYCODE_MAP(kVK_Shift, KeyCode::LeftShift)
    CRU_DEFINE_KEYCODE_MAP(kVK_RightShift, KeyCode::RightShift)
    CRU_DEFINE_KEYCODE_MAP(kVK_Control, KeyCode::LeftCtrl)
    CRU_DEFINE_KEYCODE_MAP(kVK_RightControl, KeyCode::RightCtrl)
    CRU_DEFINE_KEYCODE_MAP(kVK_Option, KeyCode::LeftAlt)
    CRU_DEFINE_KEYCODE_MAP(kVK_RightOption, KeyCode::RightAlt)
    CRU_DEFINE_KEYCODE_MAP(kVK_Command, KeyCode::LeftCommand)
    CRU_DEFINE_KEYCODE_MAP(kVK_RightCommand, KeyCode::RightCommand)
    CRU_DEFINE_KEYCODE_MAP(kVK_Delete, KeyCode::Backspace)
    CRU_DEFINE_KEYCODE_MAP(kVK_Return, KeyCode::Return)
    CRU_DEFINE_KEYCODE_MAP(kVK_ForwardDelete, KeyCode::Delete)
    CRU_DEFINE_KEYCODE_MAP(kVK_Home, KeyCode::Home)
    CRU_DEFINE_KEYCODE_MAP(kVK_End, KeyCode::End)
    CRU_DEFINE_KEYCODE_MAP(kVK_PageUp, KeyCode::PageUp)
    CRU_DEFINE_KEYCODE_MAP(kVK_PageDown, KeyCode::PageDown)
    CRU_DEFINE_KEYCODE_MAP(kVK_LeftArrow, KeyCode::Left)
    CRU_DEFINE_KEYCODE_MAP(kVK_RightArrow, KeyCode::Right)
    CRU_DEFINE_KEYCODE_MAP(kVK_UpArrow, KeyCode::Up)
    CRU_DEFINE_KEYCODE_MAP(kVK_DownArrow, KeyCode::Down)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad0, KeyCode::NumPad0)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad1, KeyCode::NumPad1)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad2, KeyCode::NumPad2)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad3, KeyCode::NumPad3)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad4, KeyCode::NumPad4)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad5, KeyCode::NumPad5)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad6, KeyCode::NumPad6)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad7, KeyCode::NumPad7)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad8, KeyCode::NumPad8)
    CRU_DEFINE_KEYCODE_MAP(kVK_ANSI_Keypad9, KeyCode::NumPad9)
    CRU_DEFINE_KEYCODE_MAP(kVK_Space, KeyCode::Space)
    default:
      return KeyCode::Unknown;
  }

#undef CRU_DEFINE_KEYCODE_MAP
}

unsigned short KeyCodeFromCruToOsx(KeyCode k) {
  switch (k) {
#define CRU_DEFINE_KEYCODE_MAP(cru, osx) \
  case cru:                              \
    return osx;
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N0, kVK_ANSI_0)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N1, kVK_ANSI_1)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N2, kVK_ANSI_2)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N3, kVK_ANSI_3)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N4, kVK_ANSI_4)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N5, kVK_ANSI_5)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N6, kVK_ANSI_6)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N7, kVK_ANSI_7)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N8, kVK_ANSI_8)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N9, kVK_ANSI_9)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::A, kVK_ANSI_A)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::B, kVK_ANSI_B)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::C, kVK_ANSI_C)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::D, kVK_ANSI_D)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::E, kVK_ANSI_E)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F, kVK_ANSI_F)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::G, kVK_ANSI_G)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::H, kVK_ANSI_H)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::I, kVK_ANSI_I)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::J, kVK_ANSI_J)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::K, kVK_ANSI_K)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::L, kVK_ANSI_L)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::M, kVK_ANSI_M)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N, kVK_ANSI_N)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::O, kVK_ANSI_O)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::P, kVK_ANSI_P)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Q, kVK_ANSI_Q)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::R, kVK_ANSI_R)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::S, kVK_ANSI_S)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::T, kVK_ANSI_T)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::U, kVK_ANSI_U)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::V, kVK_ANSI_V)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::W, kVK_ANSI_W)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::X, kVK_ANSI_X)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Y, kVK_ANSI_Y)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Z, kVK_ANSI_Z)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Comma, kVK_ANSI_Comma)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Period, kVK_ANSI_Period)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Slash, kVK_ANSI_Slash)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Semicolon, kVK_ANSI_Semicolon)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Quote, kVK_ANSI_Quote)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::LeftSquareBracket, kVK_ANSI_LeftBracket)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::RightSquareBracket, kVK_ANSI_RightBracket)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Minus, kVK_ANSI_Minus)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Equal, kVK_ANSI_Equal)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::BackSlash, kVK_ANSI_Backslash)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::GraveAccent, kVK_ANSI_Grave)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Escape, kVK_Escape)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Tab, kVK_Tab)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::CapsLock, kVK_CapsLock)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::LeftShift, kVK_Shift)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::RightShift, kVK_RightShift)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::LeftCtrl, kVK_Control)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::RightCtrl, kVK_RightControl)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::LeftAlt, kVK_Option)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::RightAlt, kVK_RightOption)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::LeftCommand, kVK_Command)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::RightCommand, kVK_RightCommand)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Backspace, kVK_Delete)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Return, kVK_Return)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Delete, kVK_ForwardDelete)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Home, kVK_Home)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::End, kVK_End)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::PageUp, kVK_PageUp)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::PageDown, kVK_PageDown)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Left, kVK_LeftArrow)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Right, kVK_RightArrow)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Up, kVK_UpArrow)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Down, kVK_DownArrow)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad0, kVK_ANSI_Keypad0)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad1, kVK_ANSI_Keypad1)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad2, kVK_ANSI_Keypad2)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad3, kVK_ANSI_Keypad3)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad4, kVK_ANSI_Keypad4)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad5, kVK_ANSI_Keypad5)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad6, kVK_ANSI_Keypad6)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad7, kVK_ANSI_Keypad7)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad8, kVK_ANSI_Keypad8)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::NumPad9, kVK_ANSI_Keypad9)
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Space, kVK_Space)
    default:
      return 0;
  }
#undef CRU_DEFINE_KEYCODE_MAP
}

NSString* ConvertKeyCodeToKeyEquivalent(KeyCode key_code) {
#define CRU_DEFINE_KEYCODE_MAP(key_code, str) \
  case key_code:                              \
    return str;

  switch (key_code) {
    CRU_DEFINE_KEYCODE_MAP(KeyCode::A, @"a")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::B, @"b")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::C, @"c")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::D, @"d")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::E, @"e")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F, @"f")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::G, @"g")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::H, @"h")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::I, @"i")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::J, @"j")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::K, @"k")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::L, @"l")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::M, @"m")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N, @"n")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::O, @"o")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::P, @"p")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Q, @"q")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::R, @"r")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::S, @"s")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::T, @"t")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::U, @"u")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::V, @"v")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::W, @"w")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::X, @"x")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Y, @"y")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Z, @"z")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N0, @"0")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N1, @"1")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N2, @"2")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N3, @"3")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N4, @"4")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N5, @"5")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N6, @"6")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N7, @"7")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N8, @"8")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::N9, @"9")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F1, @"F1")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F2, @"F2")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F3, @"F3")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F4, @"F4")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F5, @"F5")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F6, @"F6")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F7, @"F7")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F8, @"F8")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F9, @"F9")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F10, @"F10")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F11, @"F11")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::F12, @"F12")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Minus, @"-")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Equal, @"=")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Comma, @",")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Period, @".")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Slash, @"/")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Semicolon, @";")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Quote, @"'")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::LeftSquareBracket, @"[")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::RightSquareBracket, @"]")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::BackSlash, @"\\")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::GraveAccent, @"`")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Return, @"\n")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Escape, @"\e")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Tab, @"\t")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Backspace, @"\x08")
    CRU_DEFINE_KEYCODE_MAP(KeyCode::Delete, @"\x7F")
    default:
      throw Exception("Failed to convert key code to key equivalent string.");
  }
#undef CRU_DEFINE_KEYCODE_MAP
}

NSEventModifierFlags ConvertKeyModifier(KeyModifier k) {
  NSEventModifierFlags flags = 0;
  if (k & KeyModifiers::Shift) {
    flags |= NSEventModifierFlagShift;
  }
  if (k & KeyModifiers::Ctrl) {
    flags |= NSEventModifierFlagControl;
  }
  if (k & KeyModifiers::Alt) {
    flags |= NSEventModifierFlagOption;
  }
  if (k & KeyModifiers::Command) {
    flags |= NSEventModifierFlagCommand;
  }
  return flags;
}
}  // namespace cru::platform::gui::osx
