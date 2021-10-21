#pragma once
#include "cru/osx/gui/Window.hpp"

#include "cru/common/Event.hpp"
#include "cru/osx/gui/Cursor.hpp"
#include "cru/platform/gui/TimerHelper.hpp"

#import <AppKit/AppKit.h>

@interface CruWindowDelegate : NSObject <NSWindowDelegate>
- (id)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p;
@end

@interface CruWindow : NSWindow
- (instancetype)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p
         contentRect:(NSRect)contentRect
               style:(NSWindowStyleMask)style;

- (void)mouseMoved:(NSEvent*)event;
- (void)mouseEntered:(NSEvent*)event;
- (void)mouseExited:(NSEvent*)event;
- (void)mouseDown:(NSEvent*)event;
- (void)mouseUp:(NSEvent*)event;
- (void)rightMouseDown:(NSEvent*)event;
- (void)rightMouseUp:(NSEvent*)event;
- (void)scrollWheel:(NSEvent*)event;
- (void)keyDown:(NSEvent*)event;
- (void)keyUp:(NSEvent*)event;
@end

@interface CruView : NSView
- (instancetype)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p
               frame:(cru::platform::Rect)frame;
@end

namespace cru::platform::gui::osx {

namespace details {
class OsxInputMethodContextPrivate;

class OsxWindowPrivate {
  friend OsxWindow;
  friend OsxInputMethodContextPrivate;

 public:
  explicit OsxWindowPrivate(OsxWindow* osx_window) : osx_window_(osx_window) {}

  CRU_DELETE_COPY(OsxWindowPrivate)
  CRU_DELETE_MOVE(OsxWindowPrivate)

  ~OsxWindowPrivate() = default;

 public:
  void OnMouseEnterLeave(MouseEnterLeaveType type);
  void OnMouseMove(Point p);
  void OnMouseDown(MouseButton button, Point p, KeyModifier key_modifier);
  void OnMouseUp(MouseButton button, Point p, KeyModifier key_modifier);
  void OnMouseWheel(float delta, Point p, KeyModifier key_modifier);
  void OnKeyDown(KeyCode key, KeyModifier key_modifier);
  void OnKeyUp(KeyCode key, KeyModifier key_modifier);

  void OnWindowWillClose();
  void OnWindowDidExpose();
  void OnWindowDidUpdate();
  void OnWindowDidResize();

  CGLayerRef GetDrawLayer() { return draw_layer_; }

  NSWindow* GetNSWindow() { return window_; }

 private:
  void UpdateCursor();

 private:
  OsxWindow* osx_window_;

  INativeWindow* parent_;

  bool frame_;
  Rect content_rect_;

  NSWindow* window_;
  CruWindowDelegate* window_delegate_;

  CGLayerRef draw_layer_;

  bool mouse_in_ = false;

  std::shared_ptr<OsxCursor> cursor_ = nullptr;

  std::unique_ptr<OsxInputMethodContext> input_method_context_;

  TimerAutoCanceler draw_timer_;

  Event<std::nullptr_t> destroy_event_;
  Event<std::nullptr_t> paint_event_;
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
