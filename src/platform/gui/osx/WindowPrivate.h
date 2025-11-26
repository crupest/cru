#pragma once
#include "cru/platform/gui/osx/Window.h"

#include "cru/base/Event.h"
#include "cru/platform/gui/UiApplication.h"
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

class OsxWindowPrivate : public Object {
  friend CruView;
  friend OsxWindow;
  friend OsxInputMethodContextPrivate;

 public:
  explicit OsxWindowPrivate(OsxWindow* osx_window);
  ~OsxWindowPrivate();

  Rect GetClientRect();
  void SetClientRect(const Rect& rect);
  Rect GetWindowRect();
  void SetWindowRect(const Rect& rect);

 public:
  void OnPaint(const Rect& rect);
  void OnMouseEnterLeave(MouseEnterLeaveType type);
  void OnMouseMove(NSPoint p);
  void OnMouseDown(MouseButton button, NSPoint p, KeyModifier key_modifier);
  void OnMouseUp(MouseButton button, NSPoint p, KeyModifier key_modifier);
  void OnMouseWheel(float delta, NSPoint p, KeyModifier key_modifier, bool horizontal);
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
  NSWindow* GetNSWindow() { return ns_window_; }
  Point TransformWindow(const NSPoint& point);
  Rect TransformWindow(const NSRect& rect);
  NSPoint TransformWindow(const Point& point);
  NSRect TransformWindow(const Rect& rect);
  Point TransformScreen(const NSPoint& point);
  Rect TransformScreen(const NSRect& rect);
  NSPoint TransformScreen(const Point& point);
  NSRect TransformScreen(const Rect& rect);

  Size GetScreenSize();

 private:
  void CreateWindow();
  void UpdateCursor();
  CGLayerRef CreateLayer(const CGSize& size);
  Rect DoGetClientRect();

 private:
  OsxWindow* osx_window_;

  INativeWindow* parent_;
  WindowStyleFlag style_flag_;
  std::string title_;
  Rect content_rect_;

  NSWindow* ns_window_;
  NSView* ns_view_;
  CruWindowDelegate* window_delegate_;
  CGLayerRef draw_layer_;

  bool mouse_in_;

  std::shared_ptr<OsxCursor> cursor_;
  std::unique_ptr<OsxInputMethodContext> input_method_context_;
};
}  // namespace details
}  // namespace cru::platform::gui::osx
