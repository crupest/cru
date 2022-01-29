#pragma once
#include "Resource.hpp"

#include "WindowNativeMessageEventArgs.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Window.hpp"
#include "cru/win/graphics/direct/WindowRenderTarget.hpp"

#include <memory>

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API WinNativeWindow : public WinNativeResource,
                                        public virtual INativeWindow {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::platform::gui::win::WinNativeWindow")

 public:
  explicit WinNativeWindow(WinUiApplication* application);

  CRU_DELETE_COPY(WinNativeWindow)
  CRU_DELETE_MOVE(WinNativeWindow)

  ~WinNativeWindow() override;

 public:
  void Close() override;

  WinNativeWindow* GetParent() override { return parent_window_; }
  void SetParent(INativeWindow* parent) override;

  WindowStyleFlag GetStyleFlag() override { return style_flag_; }
  void SetStyleFlag(WindowStyleFlag flag) override;

  String GetTitle() override;
  void SetTitle(String title) override;

  WindowVisibilityType GetVisibility() override { return visibility_; }
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

  IEvent<std::nullptr_t>* CreateEvent() override { return &create_event_; }
  IEvent<std::nullptr_t>* DestroyEvent() override { return &destroy_event_; }
  IEvent<std::nullptr_t>* PaintEvent() override { return &paint_event_; }
  IEvent<WindowVisibilityType>* VisibilityChangeEvent() override {
    return &visibility_change_event_;
  }
  IEvent<Size>* ResizeEvent() override { return &resize_event_; }
  IEvent<FocusChangeType>* FocusEvent() override { return &focus_event_; }
  IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() override {
    return &mouse_enter_leave_event_;
  }
  IEvent<Point>* MouseMoveEvent() override { return &mouse_move_event_; }
  IEvent<platform::gui::NativeMouseButtonEventArgs>* MouseDownEvent() override {
    return &mouse_down_event_;
  }
  IEvent<platform::gui::NativeMouseButtonEventArgs>* MouseUpEvent() override {
    return &mouse_up_event_;
  }
  IEvent<NativeMouseWheelEventArgs>* MouseWheelEvent() override {
    return &mouse_wheel_event_;
  }

  IEvent<platform::gui::NativeKeyEventArgs>* KeyDownEvent() override {
    return &key_down_event_;
  }
  IEvent<platform::gui::NativeKeyEventArgs>* KeyUpEvent() override {
    return &key_up_event_;
  }

  IEvent<WindowNativeMessageEventArgs&>* NativeMessageEvent() {
    return &native_message_event_;
  }

  IInputMethodContext* GetInputMethodContext() override;

  // Get the handle of the window. Return null if window is invalid.
  HWND GetWindowHandle() const { return hwnd_; }

  bool HandleNativeWindowMessage(HWND hwnd, UINT msg, WPARAM w_param,
                                 LPARAM l_param, LRESULT* result);

  graphics::win::direct::D2DWindowRenderTarget* GetWindowRenderTarget() const {
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
  WindowVisibilityType visibility_ = WindowVisibilityType::Hide;
  Rect client_rect_{100, 100, 400, 300};

  HWND hwnd_ = nullptr;
  WinNativeWindow* parent_window_ = nullptr;

  float dpi_;

  bool has_focus_ = false;
  bool is_mouse_in_ = false;

  std::unique_ptr<graphics::win::direct::D2DWindowRenderTarget>
      window_render_target_;

  std::shared_ptr<WinCursor> cursor_;

  std::unique_ptr<WinInputMethodContext> input_method_context_;

  Event<std::nullptr_t> create_event_;
  Event<std::nullptr_t> destroy_event_;
  Event<std::nullptr_t> paint_event_;
  Event<Size> resize_event_;
  Event<WindowVisibilityType> visibility_change_event_;
  Event<FocusChangeType> focus_event_;
  Event<MouseEnterLeaveType> mouse_enter_leave_event_;
  Event<Point> mouse_move_event_;
  Event<platform::gui::NativeMouseButtonEventArgs> mouse_down_event_;
  Event<platform::gui::NativeMouseButtonEventArgs> mouse_up_event_;
  Event<platform::gui::NativeMouseWheelEventArgs> mouse_wheel_event_;
  Event<platform::gui::NativeKeyEventArgs> key_down_event_;
  Event<platform::gui::NativeKeyEventArgs> key_up_event_;

  Event<WindowNativeMessageEventArgs&> native_message_event_;
};
}  // namespace cru::platform::gui::win
