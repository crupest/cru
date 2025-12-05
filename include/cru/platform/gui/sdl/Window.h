#pragma once
#include "Base.h"

#include <cru/platform/GraphicsBase.h>
#include <cru/platform/gui/UiApplication.h>
#include <cru/platform/gui/Window.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

#ifdef __unix
#include "OpenGLRenderer.h"
#endif

namespace cru::platform::gui::sdl {
class SdlUiApplication;
class SdlInputMethodContext;

class SdlWindow : public SdlResource, public virtual INativeWindow {
 private:
  constexpr static auto kLogTag = "cru::platform::gui::xcb::SdlWindow";

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

  /**
   * Note: When the window is not created, this method will return an empty rect
   * regardless of whether the window has a border.
   */
  Rect GetWindowRect() override;

  /**
   * Note: When the window is not created, the border is always considered empty
   * regardless of the window style flag. So, you should always call this method
   * after the window is created.
   */
  void SetWindowRect(const Rect& rect) override;

  /**
   * Note: SDL3 doesn't have an API to solely set focus without raising the
   * window. So this method actually calls SDL_RaiseWindow to raise and set
   * focus to the window.
   */
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
  SDL_Window* GetSdlWindow();
  SDL_WindowID GetSdlWindowId();
  SdlUiApplication* GetSdlUiApplication();
  float GetDisplayScale();
  Thickness GetBorderThickness();

 private:
  void DoCreateWindow();
  void DoUpdateClientRect();
  void DoUpdateParent();
  void DoUpdateStyleFlag();
  void DoUpdateTitle();
  void DoUpdateCursor();

  bool HandleEvent(const SDL_Event* event);

 private:
  SdlUiApplication* application_;
  SDL_Window* sdl_window_;
  SDL_WindowID sdl_window_id_;
  Rect client_rect_;
  SdlWindow* parent_;
  EventHandlerRevokerGuard parent_create_guard_;
  WindowStyleFlag style_;
  std::string title_;
  std::shared_ptr<ICursor> cursor_;

  std::unique_ptr<SdlInputMethodContext> input_context_;

#ifdef __unix
 private:
  void UnixOnCreate(int width, int height);
  void UnixOnDestroy();
  void UnixOnResize(int width, int height);

 private:
  std::unique_ptr<SdlOpenGLRenderer> renderer_;
  TimerAutoCanceler repaint_timer_canceler_;
#endif
};
}  // namespace cru::platform::gui::sdl
