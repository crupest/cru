#pragma once
#include <cru/base/Base.h>
#include <cru/base/platform/win/Base.h>
#include <cru/platform/Base.h>

#ifdef CRU_IS_DLL
#ifdef CRU_WIN_GUI_EXPORT_API
#define CRU_WIN_GUI_API __declspec(dllexport)
#else
#define CRU_WIN_GUI_API __declspec(dllimport)
#endif
#else
#define CRU_WIN_GUI_API
#endif

namespace cru::platform::gui::win {
using platform::win::HResultError;
using platform::win::Win32Error;

class GodWindow;
class TimerManager;
class WinCursor;
class WinCursorManager;
class WindowClass;
class WindowManager;
class WinNativeWindow;
class WinUiApplication;
class WinInputMethodContext;
class WinClipboard;

class CRU_WIN_GUI_API WinNativeResource : public Object,
                                          public virtual IPlatformResource {
 public:
  static std::string kPlatformId;

 protected:
  WinNativeResource() = default;

 public:
  std::string GetPlatformId() const final { return kPlatformId; }
};
}  // namespace cru::platform::gui::win
