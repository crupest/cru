#pragma once
#include "cru/base/Base.h"
#include "cru/base/Bitmask.h"
#include "cru/platform/graphics/Base.h"

#include "../Resource.h"

#ifdef CRU_IS_DLL
#ifdef CRU_PLATFORM_GUI_EXPORT_API
#define CRU_PLATFORM_GUI_API __declspec(dllexport)
#else
#define CRU_PLATFORM_GUI_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_GUI_API
#endif

namespace cru::platform::gui {
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
struct MouseButtons {
  constexpr static MouseButton None = MouseButton::FromOffset(0);
  constexpr static MouseButton Left = MouseButton::FromOffset(1);
  constexpr static MouseButton Middle = MouseButton::FromOffset(2);
  constexpr static MouseButton Right = MouseButton::FromOffset(3);
};

namespace mouse_buttons {
constexpr MouseButton left{MouseButtons::Left};
constexpr MouseButton middle{MouseButtons::Middle};
constexpr MouseButton right{MouseButtons::Right};
}  // namespace mouse_buttons
}  // namespace cru::platform::gui
