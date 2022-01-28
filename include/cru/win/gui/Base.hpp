#pragma once
#include "../WinPreConfig.hpp"

#include "cru/common/Base.hpp"

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_WIN_GUI_EXPORT_API
#define CRU_WIN_GUI_API __declspec(dllexport)
#else
#define CRU_WIN_GUI_API __declspec(dllimport)
#endif
#else
#define CRU_WIN_GUI_API
#endif

namespace cru::platform::gui::win {
class GodWindow;
class TimerManager;
class WinCursor;
class WinCursorManager;
class WindowClass;
class WindowManager;
class WinNativeWindow;
class WinUiApplication;
class WinInputMethodContext;
}  // namespace cru::platform::gui::win
