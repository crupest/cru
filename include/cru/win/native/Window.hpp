#pragma once
#include "Resource.hpp"

#include "WindowNativeMessageEventArgs.hpp"
#include "cru/platform/GraphBase.hpp"
#include "cru/platform/native/Window.hpp"

#include <memory>

namespace cru::platform::native::win {
class WinNativeWindow : public WinNativeResource, public virtual INativeWindow {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::platform::native::win::WinNativeWindow")

 public:
  WinNativeWindow(WinUiApplication* application, WindowClass* window_class,
                  DWORD window_style, WinNativeWindow* parent);

  CRU_DELETE_COPY(WinNativeWindow)
  CRU_DELETE_MOVE(WinNativeWindow)

  ~WinNativeWindow() override;

 public:
  std::shared_ptr<INativeWindowResolver> GetResolver() override {
    return std::static_pointer_cast<INativeWindowResolver>(resolver_);
  }

  void Close() override;

  WinNativeWindow* GetParent() override { return parent_window_; }

  bool IsVisible() override;
  void SetVisible(bool is_visible) override;

  Size GetClientSize() override;
  void SetClientSize(const Size& size) override;

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  Rect GetWindowRect() override;

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  void SetWindowRect(const Rect& rect) override;

  Point GetMousePosition() override;

  bool CaptureMouse() override;
  bool ReleaseMouse() override;

  void RequestRepaint() override;
  std::unique_ptr<graph::IPainter> BeginPaint() override;

  void SetCursor(std::shared_ptr<ICursor> cursor) override;

  IEvent<std::nullptr_t>* DestroyEvent() override { return &destroy_event_; }
  IEvent<std::nullptr_t>* PaintEvent() override { return &paint_event_; }
  IEvent<Size>* ResizeEvent() override { return &resize_event_; }
  IEvent<FocusChangeType>* FocusEvent() override { return &focus_event_; }
  IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() override {
    return &mouse_enter_leave_event_;
  }
  IEvent<Point>* MouseMoveEvent() override { return &mouse_move_event_; }
  IEvent<platform::native::NativeMouseButtonEventArgs>* MouseDownEvent()
      override {
    return &mouse_down_event_;
  }
  IEvent<platform::native::NativeMouseButtonEventArgs>* MouseUpEvent()
      override {
    return &mouse_up_event_;
  }
  IEvent<platform::native::NativeKeyEventArgs>* KeyDownEvent() override {
    return &key_down_event_;
  }
  IEvent<platform::native::NativeKeyEventArgs>* KeyUpEvent() override {
    return &key_up_event_;
  }

  IEvent<WindowNativeMessageEventArgs&>* NativeMessageEvent() {
    return &native_message_event_;
  }

  // Get the handle of the window. Return null if window is invalid.
  HWND GetWindowHandle() const { return hwnd_; }

  bool HandleNativeWindowMessage(HWND hwnd, UINT msg, WPARAM w_param,
                                 LPARAM l_param, LRESULT* result);

  WindowRenderTarget* GetWindowRenderTarget() const {
    return window_render_target_.get();
  }

  //*************** region: dpi ***************
  float GetDpi() const { return dpi_; }

  inline int DipToPixel(const float dip) {
    return static_cast<int>(dip * GetDpi() / 96.0f);
  }

  inline POINT DipToPixel(const Point& dip_point) {
    POINT result;
    result.x = DipToPixel(dip_point.x);
    result.y = DipToPixel(dip_point.y);
    return result;
  }

  inline float PixelToDip(const int pixel) {
    return static_cast<float>(pixel) * 96.0f / GetDpi();
  }

  inline Point PixelToDip(const POINT& pi_point) {
    return Point(PixelToDip(pi_point.x), PixelToDip(pi_point.y));
  }

 private:
  // Get the client rect in pixel.
  RECT GetClientRectPixel();

  //*************** region: native messages ***************

  void OnDestroyInternal();
  void OnPaintInternal();
  void OnResizeInternal(int new_width, int new_height);

  void OnSetFocusInternal();
  void OnKillFocusInternal();

  void OnMouseMoveInternal(POINT point);
  void OnMouseLeaveInternal();
  void OnMouseDownInternal(platform::native::MouseButton button, POINT point);
  void OnMouseUpInternal(platform::native::MouseButton button, POINT point);

  void OnMouseWheelInternal(short delta, POINT point);
  void OnKeyDownInternal(int virtual_code);
  void OnKeyUpInternal(int virtual_code);

  void OnActivatedInternal();
  void OnDeactivatedInternal();

 private:
  WinUiApplication* application_;

  // when delete is called first, it set this to true to indicate
  // destroy message handler not to double delete this instance;
  // when destroy handler is called first (by user action or method
  // Close), it set this to true to indicate delete not call Close
  // again.
  bool sync_flag_ = false;

  std::shared_ptr<WinNativeWindowResolver> resolver_;

  HWND hwnd_;
  WinNativeWindow* parent_window_;

  float dpi_;

  bool has_focus_ = false;
  bool is_mouse_in_ = false;

  std::unique_ptr<WindowRenderTarget> window_render_target_;

  std::shared_ptr<WinCursor> cursor_;

  Event<std::nullptr_t> destroy_event_;
  Event<std::nullptr_t> paint_event_;
  Event<Size> resize_event_;
  Event<FocusChangeType> focus_event_;
  Event<MouseEnterLeaveType> mouse_enter_leave_event_;
  Event<Point> mouse_move_event_;
  Event<platform::native::NativeMouseButtonEventArgs> mouse_down_event_;
  Event<platform::native::NativeMouseButtonEventArgs> mouse_up_event_;
  Event<platform::native::NativeKeyEventArgs> key_down_event_;
  Event<platform::native::NativeKeyEventArgs> key_up_event_;

  Event<WindowNativeMessageEventArgs&> native_message_event_;
};

class WinNativeWindowResolver : public WinNativeResource,
                                public virtual INativeWindowResolver {
  friend WinNativeWindow::~WinNativeWindow();

 public:
  WinNativeWindowResolver(WinNativeWindow* window) : window_(window) {}

  CRU_DELETE_COPY(WinNativeWindowResolver)
  CRU_DELETE_MOVE(WinNativeWindowResolver)

  ~WinNativeWindowResolver() override = default;

 public:
  INativeWindow* Resolve() override { return window_; }

 private:
  void Reset();

 private:
  WinNativeWindow* window_;
};

WinNativeWindow* Resolve(gsl::not_null<INativeWindowResolver*> resolver);
}  // namespace cru::platform::native::win
