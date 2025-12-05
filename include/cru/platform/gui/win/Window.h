#pragma once
#include "Base.h"
#include "WindowNativeMessageEventArgs.h"

#include <cru/platform/graphics/direct2d/WindowRenderTarget.h>
#include <cru/platform/gui/Window.h>

#include <cmath>
#include <memory>

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API WinNativeWindow : public WinNativeResource,
                                        public virtual INativeWindow {
 private:
  constexpr static auto kLogTag = "cru::platform::gui::win::WinNativeWindow";

 public:
  explicit WinNativeWindow(WinUiApplication* application);

  ~WinNativeWindow() override;

 public:
  void Close() override;

  WinNativeWindow* GetParent() override { return parent_window_; }
  void SetParent(INativeWindow* parent) override;

  WindowStyleFlag GetStyleFlag() override { return style_flag_; }
  void SetStyleFlag(WindowStyleFlag flag) override;

  std::string GetTitle() override;
  void SetTitle(std::string title) override;

  WindowVisibilityType GetVisibility() override;
  void SetVisibility(WindowVisibilityType visibility) override;

  Size GetClientSize() override;
  void SetClientSize(const Size& size) override;

  Rect GetClientRect() override;
  void SetClientRect(const Rect& rect) override;

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  Rect GetWindowRect() override;

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  void SetWindowRect(const Rect& rect) override;

  bool RequestFocus() override;

  Point GetMousePosition() override;

  bool CaptureMouse() override;
  bool ReleaseMouse() override;

  void RequestRepaint() override;
  std::unique_ptr<graphics::IPainter> BeginPaint() override;

  void SetCursor(std::shared_ptr<ICursor> cursor) override;

  void SetToForeground() override;

  CRU_DEFINE_CRU_PLATFORM_GUI_I_NATIVE_WINDOW_OVERRIDE_EVENTS()
  CRU_DEFINE_EVENT(NativeMessage, WindowNativeMessageEventArgs&)

  IInputMethodContext* GetInputMethodContext() override;

  // Get the handle of the window. Return null if window is invalid.
  HWND GetWindowHandle() const { return hwnd_; }

  bool HandleNativeWindowMessage(HWND hwnd, UINT msg, WPARAM w_param,
                                 LPARAM l_param, LRESULT* result);

  graphics::direct2d::D2DWindowRenderTarget* GetWindowRenderTarget() const {
    return window_render_target_.get();
  }

  //*************** region: dpi ***************
  float GetDpi() const { return dpi_; }

  inline int DipToPixel(const float dip) {
    return static_cast<int>(std::ceil(dip * GetDpi() / 96.0f));
  }

  inline POINT DipToPixel(const Point& dip_point) {
    POINT result;
    result.x = DipToPixel(dip_point.x);
    result.y = DipToPixel(dip_point.y);
    return result;
  }

  inline RECT DipToPixel(const Rect& dip_rect) {
    RECT result;
    result.left = DipToPixel(dip_rect.left);
    result.top = DipToPixel(dip_rect.top);
    result.right = DipToPixel(dip_rect.GetRight());
    result.bottom = DipToPixel(dip_rect.GetBottom());
    return result;
  }

  inline float PixelToDip(const int pixel) {
    return static_cast<float>(pixel) * 96.0f / GetDpi();
  }

  inline Point PixelToDip(const POINT& pi_point) {
    return Point(PixelToDip(pi_point.x), PixelToDip(pi_point.y));
  }

  inline Rect PixelToDip(const RECT& pi_rect) {
    return Rect::FromVertices(PixelToDip(pi_rect.left), PixelToDip(pi_rect.top),
                              PixelToDip(pi_rect.right),
                              PixelToDip(pi_rect.bottom));
  }

 private:
  // Get the client rect in pixel.
  RECT GetClientRectPixel();

  void RecreateWindow();

  //*************** region: native messages ***************

  void OnCreateInternal();
  void OnDestroyInternal();
  void OnPaintInternal();
  void OnMoveInternal(int new_left, int new_top);
  void OnResizeInternal(int new_width, int new_height);

  void OnSetFocusInternal();
  void OnKillFocusInternal();

  void OnMouseMoveInternal(POINT point);
  void OnMouseLeaveInternal();
  void OnMouseDownInternal(platform::gui::MouseButton button, POINT point);
  void OnMouseUpInternal(platform::gui::MouseButton button, POINT point);

  void OnMouseWheelInternal(short delta, POINT point);
  void OnKeyDownInternal(int virtual_code);
  void OnKeyUpInternal(int virtual_code);

  void OnActivatedInternal();
  void OnDeactivatedInternal();

 private:
  WinUiApplication* application_;

  WindowStyleFlag style_flag_{};
  Rect client_rect_{100, 100, 400, 300};
  std::string title_;

  HWND hwnd_ = nullptr;
  WinNativeWindow* parent_window_ = nullptr;

  float dpi_;

  bool has_focus_ = false;
  bool is_mouse_in_ = false;

  std::unique_ptr<graphics::direct2d::D2DWindowRenderTarget>
      window_render_target_;

  std::shared_ptr<WinCursor> cursor_;

  std::unique_ptr<WinInputMethodContext> input_method_context_;
};
}  // namespace cru::platform::gui::win
