#pragma once
#include <cru/Base.h>
#include <cru/Bitmask.h>
#include <cru/Platform.h>

#include <cru/graphics/Base.h>

// TODO: Update this macro.
// Current: CRU_GUI_API 
// Removed: CRU_PLATFORM_GUI_API 

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_GUI_EXPORT_API
#define CRU_GUI_API __declspec(dllexport)
#else
#define CRU_GUI_API __declspec(dllimport)
#endif
#else
#define CRU_GUI_API
#endif
#define CRU_PLATFORM_GUI_API CRU_GUI_API

namespace cru::platform::gui {

// Import namespace
using cru::graphics::Point;
using cru::graphics::Size;
using cru::graphics::Rect;


struct ICursor;
struct ICursorManager;
struct IUiApplication;
struct INativeWindow;
struct IInputMethodContext;
struct IClipboard;

namespace details {
struct TagMouseButton {};
}  // namespace details

using MouseButton = Bitmask<details::TagMouseButton>;

namespace mouse_buttons {
constexpr MouseButton left{0b1};
constexpr MouseButton middle{0b10};
constexpr MouseButton right{0b100};
}  // namespace mouse_buttons
}  // namespace cru::platform::gui
