#pragma once
#include "../../GraphicsBase.h"
#include "../Window.h"
#include "Base.h"

#include <SDL3/SDL_video.h>
#include <optional>

namespace cru::platform::gui::sdl {
class SdlUiApplication;

class SdlWindow : public SdlResource, public virtual INativeWindow {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::gui::xcb::SdlWindow")

  friend SdlUiApplication;

 public:
  explicit SdlWindow(SdlUiApplication* application);
  ~SdlWindow() override;

  bool IsCreated() override;
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

  CRU_DEFINE_CRU_PLATFORM_GUI_I_NATIVE_WINDOW_OVERRIDE_EVENTS()

  IInputMethodContext* GetInputMethodContext() override;

 public:
  std::optional<SDL_Window*> GetSdlWindow();
  SdlUiApplication* GetSdlUiApplication();
  float GetDisplayScale();
  Thickness GetBorderThickness();

 private:
  SdlUiApplication* application_;
  std::optional<SDL_Window*> sdl_window_;
  Rect client_rect_;
  SdlWindow* parent_;
  WindowStyleFlag style_;
};
}  // namespace cru::platform::gui::sdl
