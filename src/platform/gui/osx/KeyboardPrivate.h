#pragma once
#include "cru/platform/gui/osx/Keyboard.h"

#import <AppKit/NSEvent.h>

namespace cru::platform::gui::osx {
NSString* ConvertKeyCodeToKeyEquivalent(KeyCode key_code);
NSEventModifierFlags ConvertKeyModifier(KeyModifier k);
}
