#pragma once
#include "resource.hpp"

#include "cru/platform/native/window.hpp"
#include "window_native_message_event_args.hpp"

#include <memory>

namespace cru::platform::native::win {
class WinNativeWindow : public WinNativeResource, public virtual INativeWindow {
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
  IEvent<std::string>* CharEvent() override { return &char_event_; };

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
  void OnCharInternal(wchar_t c);

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
  Event<std::string> char_event_;

  Event<WindowNativeMessageEventArgs&> native_message_event_;

  // WM_CHAR may be sent twice successively with two utf-16 code units of
  // surrogate pair when character is from supplementary planes. This field is
  // used to save the previous one.
  wchar_t last_wm_char_event_wparam_;
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
