#pragma once
#include "Base.h"

#include <cru/platform/gui/Window.h>

#include <memory>

namespace cru::platform::gui::osx {
namespace details {
class OsxWindowPrivate;
class OsxInputMethodContextPrivate;
}  // namespace details

class OsxUiApplication;
class OsxInputMethodContext;

class OsxWindow : public OsxGuiResource, public INativeWindow {
  friend details::OsxWindowPrivate;
  friend details::OsxInputMethodContextPrivate;

 public:
  OsxWindow(OsxUiApplication* ui_application);
  ~OsxWindow() override;

 public:
  void Close() override;

  INativeWindow* GetParent() override;
  void SetParent(INativeWindow* parent) override;

  WindowStyleFlag GetStyleFlag() override;
  void SetStyleFlag(WindowStyleFlag flag) override;

  std::string GetTitle() override;
  void SetTitle(std::string title) override;

  WindowVisibilityType GetVisibility() override;
  void SetVisibility(WindowVisibilityType visibility) override;

  Size GetClientSize() override;
  void SetClientSize(const Size& size) override;

  Rect GetClientRect() override;
  void SetClientRect(const Rect& rect) override;

  Rect GetWindowRect() override;
  void SetWindowRect(const Rect& rect) override;

  bool RequestFocus() override;

  Point GetMousePosition() override;

  bool CaptureMouse() override;
  bool ReleaseMouse() override;

  void SetCursor(std::shared_ptr<ICursor> cursor) override;

  void SetToForeground() override;

  void RequestRepaint() override;

  std::unique_ptr<graphics::IPainter> BeginPaint() override;

  IInputMethodContext* GetInputMethodContext() override;

  CRU_DEFINE_CRU_PLATFORM_GUI_I_NATIVE_WINDOW_OVERRIDE_EVENTS()

 private:
  std::unique_ptr<details::OsxWindowPrivate> p_;
};
}  // namespace cru::platform::gui::osx
