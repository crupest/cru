#pragma once
#include "cru/osx/gui/Keyboard.hpp"

#import <AppKit/NSEvent.h>

namespace cru::platform::gui::osx {
NSString* ConvertKeyCodeToKeyEquivalent(KeyCode key_code);
NSEventModifierFlags ConvertKeyModifier(KeyModifier k);
}
