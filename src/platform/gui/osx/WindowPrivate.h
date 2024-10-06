#pragma once
#include "cru/platform/gui/osx/Window.h"

#include "cru/base/Event.h"
#include "cru/platform/gui/TimerHelper.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/osx/Cursor.h"

#import <AppKit/AppKit.h>

@interface CruWindowDelegate : NSObject <NSWindowDelegate>
- (id)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p;
@end

@interface CruWindow : NSWindow
- (instancetype)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p
         contentRect:(NSRect)contentRect
               style:(NSWindowStyleMask)style;
@end

@interface CruView : NSView <NSTextInputClient>
- (instancetype)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p
     input_context_p:
         (cru::platform::gui::osx::details::OsxInputMethodContextPrivate*)input_context_p
               frame:(cru::platform::Rect)frame;
@end

namespace cru::platform::gui::osx {

namespace details {
class OsxInputMethodContextPrivate;

class OsxWindowPrivate {
  friend OsxWindow;
  friend OsxInputMethodContextPrivate;

 public:
  explicit OsxWindowPrivate(OsxWindow* osx_window);

  CRU_DELETE_COPY(OsxWindowPrivate)
  CRU_DELETE_MOVE(OsxWindowPrivate)

  ~OsxWindowPrivate();

 public:
  void OnMouseEnterLeave(MouseEnterLeaveType type);
  void OnMouseMove(Point p);
  void OnMouseDown(MouseButton button, Point p, KeyModifier key_modifier);
  void OnMouseUp(MouseButton button, Point p, KeyModifier key_modifier);
  void OnMouseWheel(float delta, Point p, KeyModifier key_modifier, bool horizontal);
  void OnKeyDown(KeyCode key, KeyModifier key_modifier);
  void OnKeyUp(KeyCode key, KeyModifier key_modifier);

  void OnWindowWillClose();
  void OnWindowDidExpose();
  void OnWindowDidUpdate();
  void OnWindowDidMove();
  void OnWindowDidResize();
  void OnBecomeKeyWindow();
  void OnResignKeyWindow();

  CGLayerRef GetDrawLayer() { return draw_layer_; }

  OsxWindow* GetWindow() { return osx_window_; }
  NSWindow* GetNSWindow() { return window_; }

  Size GetScreenSize();

 private:
  void CreateWindow();

  void UpdateCursor();

  Point TransformMousePoint(const Point& point);

  CGLayerRef CreateLayer(const CGSize& size);

  Rect RetrieveContentRect();

 private:
  OsxWindow* osx_window_;

  INativeWindow* parent_ = nullptr;
  WindowStyleFlag style_flag_ = WindowStyleFlag{};

  String title_;

  Rect content_rect_;

  NSWindow* window_ = nil;
  CruWindowDelegate* window_delegate_ = nil;

  CGLayerRef draw_layer_ = nullptr;

  bool mouse_in_ = false;

  std::shared_ptr<OsxCursor> cursor_ = nullptr;

  std::unique_ptr<OsxInputMethodContext> input_method_context_;

  TimerAutoCanceler draw_timer_;

  Event<std::nullptr_t> create_event_;
  Event<std::nullptr_t> destroy_event_;
  Event<std::nullptr_t> paint_event_;
  Event<WindowVisibilityType> visibility_change_event_;
  Event<Size> resize_event_;
  Event<FocusChangeType> focus_event_;
  Event<MouseEnterLeaveType> mouse_enter_leave_event_;
  Event<Point> mouse_move_event_;
  Event<NativeMouseButtonEventArgs> mouse_down_event_;
  Event<NativeMouseButtonEventArgs> mouse_up_event_;
  Event<NativeMouseWheelEventArgs> mouse_wheel_event_;
  Event<NativeKeyEventArgs> key_down_event_;
  Event<NativeKeyEventArgs> key_up_event_;
};
}  // namespace details
}  // namespace cru::platform::gui::osx
