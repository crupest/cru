#include "cru/osx/gui/Window.hpp"
#include "WindowPrivate.h"

#include "CursorPrivate.h"
#include "cru/common/Logger.hpp"
#include "cru/common/Range.hpp"
#include "cru/osx/Convert.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Painter.hpp"
#include "cru/osx/gui/Cursor.hpp"
#include "cru/osx/gui/InputMethod.hpp"
#include "cru/osx/gui/Keyboard.hpp"
#include "cru/osx/gui/Resource.hpp"
#include "cru/osx/gui/UiApplication.hpp"
#include "cru/platform/Check.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/InputMethod.hpp"
#include "cru/platform/gui/Keyboard.hpp"
#include "cru/platform/gui/TimerHelper.hpp"
#include "cru/platform/gui/Window.hpp"

#include <AppKit/NSGraphicsContext.h>
#include <AppKit/NSTextInputContext.h>
#include <AppKit/NSWindow.h>
#include <Foundation/NSAttributedString.h>
#include <Foundation/NSString.h>

#include <gsl/gsl_assert>
#include <limits>
#include <memory>

namespace cru::platform::gui::osx {
namespace {
inline NSWindowStyleMask CalcWindowStyleMask(bool frame) {
  return frame ? NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                     NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
               : NSWindowStyleMaskBorderless;
}
}

namespace details {
void OsxWindowPrivate::OnWindowWillClose() {
  osx_window_->destroy_event_.Raise(nullptr);
  window_ = nil;
  CGLayerRelease(draw_layer_);
}

void OsxWindowPrivate::OnWindowDidExpose() { osx_window_->RequestRepaint(); }
void OsxWindowPrivate::OnWindowDidUpdate() {}
void OsxWindowPrivate::OnWindowDidResize() {
  NSRect rect = [NSWindow contentRectForFrameRect:[window_ frame]
                                        styleMask:CalcWindowStyleMask(frame_)];
  content_rect_ = cru::platform::graphics::osx::quartz::Convert(rect);

  osx_window_->resize_event_.Raise(osx_window_->GetClientSize());

  CGLayerRelease(draw_layer_);
  draw_layer_ = CGLayerCreateWithContext(nullptr, rect.size, nullptr);
  Ensures(draw_layer_);

  osx_window_->RequestRepaint();
}

void OsxWindowPrivate::OnMouseEnterLeave(MouseEnterLeaveType type) {
  osx_window_->mouse_enter_leave_event_.Raise(type);
  if (type == MouseEnterLeaveType::Enter) {
    mouse_in_ = true;
    UpdateCursor();
  } else {
    mouse_in_ = false;
  }
}

void OsxWindowPrivate::OnMouseMove(Point p) { osx_window_->mouse_move_event_.Raise(p); }

void OsxWindowPrivate::OnMouseDown(MouseButton button, Point p, KeyModifier key_modifier) {
  osx_window_->mouse_down_event_.Raise({button, p, key_modifier});
}

void OsxWindowPrivate::OnMouseUp(MouseButton button, Point p, KeyModifier key_modifier) {
  osx_window_->mouse_up_event_.Raise({button, p, key_modifier});
}

void OsxWindowPrivate::OnMouseWheel(float delta, Point p, KeyModifier key_modifier) {
  osx_window_->mouse_wheel_event_.Raise({delta, p, key_modifier});
}

void OsxWindowPrivate::OnKeyDown(KeyCode key, KeyModifier key_modifier) {
  osx_window_->key_down_event_.Raise({key, key_modifier});
}

void OsxWindowPrivate::OnKeyUp(KeyCode key, KeyModifier key_modifier) {
  osx_window_->key_up_event_.Raise({key, key_modifier});
}

void OsxWindowPrivate::UpdateCursor() {
  auto cursor = cursor_ == nullptr
                    ? std::dynamic_pointer_cast<OsxCursor>(
                          osx_window_->GetUiApplication()->GetCursorManager()->GetSystemCursor(
                              SystemCursorType::Arrow))
                    : cursor_;

  [cursor->p_->ns_cursor_ set];
}
}

OsxWindow::OsxWindow(OsxUiApplication* ui_application, INativeWindow* parent, bool frame)
    : OsxGuiResource(ui_application), p_(new details::OsxWindowPrivate(this)) {
  p_->window_delegate_ = [[CruWindowDelegate alloc] init:p_.get()];

  p_->parent_ = parent;

  p_->frame_ = frame;
  p_->content_rect_ = {100, 100, 400, 200};

  p_->input_method_context_ = std::make_unique<OsxInputMethodContext>(this);
}

OsxWindow::~OsxWindow() {
  Close();
  dynamic_cast<OsxUiApplication*>(GetUiApplication())->UnregisterWindow(this);
}

void OsxWindow::Close() {
  if (p_->window_) {
    [p_->window_ close];
  }
}

INativeWindow* OsxWindow::GetParent() { return p_->parent_; }

bool OsxWindow::IsVisible() {
  if (!p_->window_) return false;
  return [p_->window_ isVisible];
}

void OsxWindow::SetVisible(bool is_visible) {
  if (p_->window_) {
    if (is_visible) {
      [p_->window_ orderFront:p_->window_];
    } else {
      [p_->window_ orderOut:p_->window_];
    }
  } else {
    if (is_visible) {
      CreateWindow();
      [p_->window_ orderFront:p_->window_];
    }
  }
}

Size OsxWindow::GetClientSize() { return p_->content_rect_.GetSize(); }

void OsxWindow::SetClientSize(const Size& size) {
  if (p_->window_) {
    [p_->window_ setContentSize:NSSize{size.width, size.height}];
  } else {
    p_->content_rect_.SetSize(size);
  }
}

Rect OsxWindow::GetWindowRect() {
  if (p_->window_) {
    auto r = [p_->window_ frame];
    return Rect(r.origin.x, r.origin.y, r.size.width, r.size.height);
  } else {
    NSRect rr{p_->content_rect_.left, p_->content_rect_.top, p_->content_rect_.width,
              p_->content_rect_.height};
    auto r = [NSWindow frameRectForContentRect:rr styleMask:CalcWindowStyleMask(p_->frame_)];
    return Rect(r.origin.x, r.origin.y, r.size.width, r.size.height);
  }
}

void OsxWindow::SetWindowRect(const Rect& rect) {
  auto rr = NSRect{rect.left, rect.top, rect.width, rect.height};
  if (p_->window_) {
    [p_->window_ setFrame:rr display:false];
  } else {
    auto r = [NSWindow contentRectForFrameRect:rr styleMask:CalcWindowStyleMask(p_->frame_)];
    p_->content_rect_ = Rect(r.origin.x, r.origin.y, r.size.width, r.size.height);
  }
}

void OsxWindow::RequestRepaint() {
  if (!p_->draw_timer_) {
    p_->draw_timer_ = GetUiApplication()->SetImmediate([this] {
      this->paint_event_.Raise(nullptr);
      this->p_->draw_timer_.Release();
    });
  }
}

std::unique_ptr<graphics::IPainter> OsxWindow::BeginPaint() {
  CGContextRef cg_context = CGLayerGetContext(p_->draw_layer_);

  return std::make_unique<cru::platform::graphics::osx::quartz::QuartzCGContextPainter>(
      GetUiApplication()->GetGraphicsFactory(), cg_context, false, GetClientSize(),
      [this](graphics::osx::quartz::QuartzCGContextPainter*) {
        log::Debug(u"Finish painting and invalidate view.");
        [[p_->window_ contentView] setNeedsDisplay:YES];
      });
}

void OsxWindow::CreateWindow() {
  NSRect content_rect = CGRectMake(p_->content_rect_.left, p_->content_rect_.top,
                                   p_->content_rect_.width, p_->content_rect_.height);

  NSWindowStyleMask style_mask = CalcWindowStyleMask(p_->frame_);

  p_->window_ = [[CruWindow alloc] init:p_.get() contentRect:content_rect style:style_mask];

  [p_->window_ setDelegate:p_->window_delegate_];

  NSView* content_view = [[CruView alloc] init:p_.get()
                                         frame:Rect(Point{}, p_->content_rect_.GetSize())];

  [p_->window_ setContentView:content_view];

  p_->draw_layer_ = CGLayerCreateWithContext(
      nullptr, cru::platform::graphics::osx::quartz::Convert(p_->content_rect_.GetSize()), nullptr);
  Ensures(p_->draw_layer_);

  RequestRepaint();
}

Point OsxWindow::GetMousePosition() {
  auto p = [p_->window_ mouseLocationOutsideOfEventStream];
  return Point(p.x, p.y);
}

bool OsxWindow::CaptureMouse() { return true; }

bool OsxWindow::ReleaseMouse() { return true; }

void OsxWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  p_->cursor_ = CheckPlatform<OsxCursor>(cursor, GetPlatformId());
  if (p_->mouse_in_) {
    p_->UpdateCursor();
  }
}

IInputMethodContext* OsxWindow::GetInputMethodContext() { return p_->input_method_context_.get(); }
}

@implementation CruWindow {
  cru::platform::gui::osx::details::OsxWindowPrivate* _p;
}

- (instancetype)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p
         contentRect:(NSRect)contentRect
               style:(NSWindowStyleMask)style {
  [super initWithContentRect:contentRect
                   styleMask:style
                     backing:NSBackingStoreBuffered
                       defer:false];
  _p = p;
  return self;
}

- (void)mouseMoved:(NSEvent*)event {
  _p->OnMouseMove(cru::platform::Point(event.locationInWindow.x, event.locationInWindow.y));
}

- (void)mouseEntered:(NSEvent*)event {
  _p->OnMouseEnterLeave(cru::platform::gui::MouseEnterLeaveType::Enter);
}

- (void)mouseExited:(NSEvent*)event {
  _p->OnMouseEnterLeave(cru::platform::gui::MouseEnterLeaveType::Leave);
}

- (void)mouseDown:(NSEvent*)event {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::shift;
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseDown(cru::platform::gui::mouse_buttons::left, p, key_modifier);
}

- (void)mouseUp:(NSEvent*)event {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::shift;
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseUp(cru::platform::gui::mouse_buttons::left, p, key_modifier);
}

- (void)rightMouseDown:(NSEvent*)event {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::shift;
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseDown(cru::platform::gui::mouse_buttons::right, p, key_modifier);
}

- (void)rightMouseUp:(NSEvent*)event {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::shift;
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseUp(cru::platform::gui::mouse_buttons::right, p, key_modifier);
}

- (void)scrollWheel:(NSEvent*)event {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::shift;
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseWheel(static_cast<float>(event.scrollingDeltaY), p, key_modifier);
}

- (void)keyDown:(NSEvent*)event {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::shift;
  auto c = cru::platform::gui::osx::KeyCodeFromOsxToCru(event.keyCode);

  _p->OnKeyDown(c, key_modifier);
}

- (void)keyUp:(NSEvent*)event {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::shift;
  auto c = cru::platform::gui::osx::KeyCodeFromOsxToCru(event.keyCode);

  _p->OnKeyUp(c, key_modifier);
}
@end

@implementation CruView {
  cru::platform::gui::osx::details::OsxWindowPrivate* _p;
}

- (instancetype)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p
               frame:(cru::platform::Rect)frame {
  [super initWithFrame:cru::platform::graphics::osx::quartz::Convert(frame)];
  _p = p;

  return self;
}

- (void)drawRect:(NSRect)dirtyRect {
  cru::log::TagDebug(u"CruView", u"Begin to draw layer in view.");
  auto cg_context = [[NSGraphicsContext currentContext] CGContext];
  auto layer = _p->GetDrawLayer();
  Ensures(layer);
  CGContextDrawLayerAtPoint(cg_context, CGPointMake(0, 0), layer);
}

@end

@implementation CruWindowDelegate {
  cru::platform::gui::osx::details::OsxWindowPrivate* _p;
}

- (id)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p {
  _p = p;
  return self;
}

- (void)windowWillClose:(NSNotification*)notification {
  _p->OnWindowWillClose();
}

- (void)windowDidExpose:(NSNotification*)notification {
  _p->OnWindowDidExpose();
}

- (void)windowDidUpdate:(NSNotification*)notification {
  _p->OnWindowDidUpdate();
}

- (void)windowDidResize:(NSNotification*)notification {
  _p->OnWindowDidResize();
}
@end
