#include "cru/osx/gui/Window.hpp"

#include "CursorPrivate.h"
#include "cru/common/Range.hpp"
#include "cru/osx/Convert.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Painter.hpp"
#include "cru/osx/gui/Cursor.hpp"
#include "cru/osx/gui/Keyboard.hpp"
#include "cru/osx/gui/Resource.hpp"
#include "cru/osx/gui/UiApplication.hpp"
#include "cru/platform/Check.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/InputMethod.hpp"
#include "cru/platform/gui/Keyboard.hpp"
#include "cru/platform/gui/Window.hpp"

#include <AppKit/NSGraphicsContext.h>
#include <AppKit/NSTextInputContext.h>
#include <AppKit/NSWindow.h>
#include <Foundation/NSAttributedString.h>
#include <Foundation/NSString.h>

#include <limits>
#include <memory>

using cru::platform::osx::Convert;

@interface WindowDelegate : NSObject <NSWindowDelegate>
- (id)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p;
@end

@interface Window : NSWindow
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

@interface InputClient : NSObject <NSTextInputClient>
- (id)init:(cru::platform::gui::osx::details::OsxInputMethodContextPrivate*)p;
@end

namespace cru::platform::gui::osx {

namespace {
inline NSWindowStyleMask CalcWindowStyleMask(bool frame) {
  return frame ? NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                     NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
               : NSWindowStyleMaskBorderless;
}
}

namespace details {
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

 private:
  void UpdateCursor();

 private:
  OsxWindow* osx_window_;

  INativeWindow* parent_;

  bool frame_;
  Rect content_rect_;

  NSWindow* window_;
  WindowDelegate* window_delegate_;
  NSGraphicsContext* graphics_context_;

  bool mouse_in_ = false;

  std::shared_ptr<OsxCursor> cursor_ = nullptr;

  std::unique_ptr<OsxInputMethodContext> input_method_context_;
};

void OsxWindowPrivate::OnWindowWillClose() {
  osx_window_->destroy_event_.Raise(nullptr);
  window_ = nil;
  graphics_context_ = nil;
}

void OsxWindowPrivate::OnWindowDidExpose() { [window_ update]; }
void OsxWindowPrivate::OnWindowDidUpdate() { osx_window_->paint_event_.Raise(nullptr); }
void OsxWindowPrivate::OnWindowDidResize() {
  osx_window_->resize_event_.Raise(osx_window_->GetClientSize());

  NSRect rect = [NSWindow contentRectForFrameRect:[window_ frame]
                                        styleMask:CalcWindowStyleMask(frame_)];

  content_rect_ = cru::platform::graphics::osx::quartz::Convert(rect);
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
  p_->window_delegate_ = [[WindowDelegate alloc] init:p_.get()];

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

void OsxWindow::RequestRepaint() { [p_->window_ update]; }

std::unique_ptr<graphics::IPainter> OsxWindow::BeginPaint() {
  CGContextRef cg_context = [p_->graphics_context_ CGContext];

  return std::make_unique<cru::platform::graphics::osx::quartz::QuartzCGContextPainter>(
      GetUiApplication()->GetGraphicsFactory(), cg_context, false, GetClientSize());
}

void OsxWindow::CreateWindow() {
  NSRect content_rect = CGRectMake(p_->content_rect_.left, p_->content_rect_.top,
                                   p_->content_rect_.width, p_->content_rect_.height);

  NSWindowStyleMask style_mask = CalcWindowStyleMask(p_->frame_);

  p_->window_ = [[Window alloc] init:p_.get() contentRect:content_rect style:style_mask];

  [p_->window_ setDelegate:p_->window_delegate_];

  p_->graphics_context_ = [NSGraphicsContext graphicsContextWithWindow:p_->window_];
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

namespace details {
class OsxInputMethodContextPrivate {
  friend OsxInputMethodContext;

 public:
  OsxInputMethodContextPrivate(OsxInputMethodContext* input_method_context, OsxWindow* window);

  CRU_DELETE_COPY(OsxInputMethodContextPrivate)
  CRU_DELETE_MOVE(OsxInputMethodContextPrivate)

  ~OsxInputMethodContextPrivate();

  void SetCompositionText(CompositionText composition_text) {
    composition_text_ = std::move(composition_text);
  }

  void RaiseCompositionStartEvent();
  void RaiseCompositionEndEvent();
  void RaiseCompositionEvent();
  void RaiseTextEvent(StringView text);

  Point GetCandidateWindowPosition() const { return candidate_window_point_; }
  void SetCandidateWindowPosition(const Point& p) { candidate_window_point_ = p; }

  Range GetSelectionRange() const { return selection_range_; }
  void SetSelectionRange(Range selection_range) { selection_range_ = selection_range; }

  void PerformSel(SEL sel);

 private:
  OsxWindow* window_;

  CompositionText composition_text_;

  Range selection_range_;

  OsxInputMethodContext* input_method_context_;

  // On Osx, this is the text lefttop point on screen.
  Point candidate_window_point_;

  Event<std::nullptr_t> composition_start_event_;
  Event<std::nullptr_t> composition_event_;
  Event<std::nullptr_t> composition_end_event_;
  Event<StringView> text_event_;
};

OsxInputMethodContextPrivate::OsxInputMethodContextPrivate(
    OsxInputMethodContext* input_method_context, OsxWindow* window) {
  input_method_context_ = input_method_context;
  window_ = window;
}

OsxInputMethodContextPrivate::~OsxInputMethodContextPrivate() {}

void OsxInputMethodContextPrivate::RaiseCompositionStartEvent() {
  composition_start_event_.Raise(nullptr);
}
void OsxInputMethodContextPrivate::RaiseCompositionEndEvent() {
  composition_end_event_.Raise(nullptr);
}
void OsxInputMethodContextPrivate::RaiseCompositionEvent() { composition_event_.Raise(nullptr); }

void OsxInputMethodContextPrivate::RaiseTextEvent(StringView text) { text_event_.Raise(text); }

void OsxInputMethodContextPrivate::PerformSel(SEL sel) {
  [window_->p_->window_ performSelector:sel];
}

}

OsxInputMethodContext::OsxInputMethodContext(OsxWindow* window)
    : OsxGuiResource(window->GetUiApplication()) {
  p_ = std::make_unique<details::OsxInputMethodContextPrivate>(this, window);
}

OsxInputMethodContext::~OsxInputMethodContext() {}

void OsxInputMethodContext::EnableIME() { [[NSTextInputContext currentInputContext] deactivate]; }

void OsxInputMethodContext::DisableIME() { [[NSTextInputContext currentInputContext] activate]; }

bool OsxInputMethodContext::ShouldManuallyDrawCompositionText() { return true; }

void OsxInputMethodContext::CompleteComposition() {
  // TODO: Implement this.
}

void OsxInputMethodContext::CancelComposition() {
  [[NSTextInputContext currentInputContext] discardMarkedText];
}

CompositionText OsxInputMethodContext::GetCompositionText() { return p_->composition_text_; }

void OsxInputMethodContext::SetCandidateWindowPosition(const Point& point) {
  p_->SetCandidateWindowPosition(point);
}

IEvent<std::nullptr_t>* OsxInputMethodContext::CompositionStartEvent() {
  return &p_->composition_start_event_;
}

IEvent<std::nullptr_t>* OsxInputMethodContext::CompositionEndEvent() {
  return &p_->composition_end_event_;
}

IEvent<std::nullptr_t>* OsxInputMethodContext::CompositionEvent() {
  return &p_->composition_event_;
}

IEvent<StringView>* OsxInputMethodContext::TextEvent() { return &p_->text_event_; }
}

@implementation Window {
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

@implementation WindowDelegate {
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

@implementation InputClient {
  cru::platform::gui::osx::details::OsxInputMethodContextPrivate* _p;
  NSMutableAttributedString* _text;
}

- (id)init:(cru::platform::gui::osx::details::OsxInputMethodContextPrivate*)p {
  _p = p;
  return self;
}

- (BOOL)hasMarkedText {
  return _text != nil;
}

- (NSRange)markedRange {
  return _text == nil ? NSRange{NSNotFound, 0} : NSRange{0, [_text length]};
}

- (NSRange)selectedRange {
  return NSMakeRange(_p->GetSelectionRange().position, _p->GetSelectionRange().count);
}

- (void)setMarkedText:(id)string
        selectedRange:(NSRange)selectedRange
     replacementRange:(NSRange)replacementRange {
  if (_text == nil) {
    _text = [[NSMutableAttributedString alloc] init];
    _p->RaiseCompositionStartEvent();
  }

  [_text deleteCharactersInRange:replacementRange];
  [_text insertAttributedString:[[NSAttributedString alloc] initWithString:(NSString*)string]
                        atIndex:replacementRange.location];

  cru::platform::gui::CompositionText composition_text;
  composition_text.text = Convert((CFStringRef)[_text string]);
  composition_text.selection.position = replacementRange.location + selectedRange.location;
  composition_text.selection.count = selectedRange.length;
  _p->SetCompositionText(composition_text);
  _p->RaiseCompositionEvent();
}

- (void)unmarkText {
  _text = nil;
  _p->RaiseCompositionEndEvent();
}

- (NSArray<NSAttributedStringKey>*)validAttributesForMarkedText {
  return @[
    (NSString*)kCTUnderlineColorAttributeName, (NSString*)kCTUnderlineStyleAttributeName,
    (NSString*)kCTForegroundColorAttributeName, (NSString*)kCTBackgroundColorAttributeName
  ];
}

- (NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range
                                               actualRange:(NSRangePointer)actualRange {
  return [_text attributedSubstringFromRange:range];
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {
  _text = nil;
  cru::String s = Convert((CFStringRef)string);
  _p->RaiseCompositionEndEvent();
  _p->RaiseTextEvent(s);
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point {
  return NSNotFound;
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actualRange {
  NSRect result;
  result.origin.x = _p->GetCandidateWindowPosition().x;
  result.origin.y = _p->GetCandidateWindowPosition().y;
  result.size.height = 16;
  result.size.width = 0;
  return result;
}

- (void)doCommandBySelector:(SEL)selector {
  _p->PerformSel(selector);
}
@end
