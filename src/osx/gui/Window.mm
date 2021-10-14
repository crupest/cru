#include "cru/osx/gui/Window.hpp"

#include "cru/common/Range.hpp"
#include "cru/osx/Convert.hpp"
#include "cru/osx/graphics/quartz/Painter.hpp"
#include "cru/osx/gui/Keyboard.hpp"
#include "cru/osx/gui/UiApplication.hpp"
#include "cru/platform/gui/Base.hpp"
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

@interface InputClient : NSObject <NSTextInputClient>
- (id)init:(cru::platform::gui::osx::details::OsxInputMethodContextPrivate*)p;
@end

namespace cru::platform::gui::osx {
namespace details {

class OsxWindowPrivate {
  friend OsxWindow;

 public:
  explicit OsxWindowPrivate(OsxWindow* osx_window) : osx_window_(osx_window) {
    window_delegate_ = [[WindowDelegate alloc] init:this];
  }

  CRU_DELETE_COPY(OsxWindowPrivate)
  CRU_DELETE_MOVE(OsxWindowPrivate)

  ~OsxWindowPrivate() = default;

 public:
  void OnWindowWillClose();
  void OnWindowDidExpose();
  void OnWindowDidUpdate();
  void OnWindowDidResize();

 private:
  OsxWindow* osx_window_;

  INativeWindow* parent_;

  bool frame_;
  Rect content_rect_;

  NSWindow* window_;
  WindowDelegate* window_delegate_;
};

void OsxWindowPrivate::OnWindowWillClose() { osx_window_->destroy_event_.Raise(nullptr); }
void OsxWindowPrivate::OnWindowDidExpose() { [window_ update]; }
void OsxWindowPrivate::OnWindowDidUpdate() { osx_window_->paint_event_.Raise(nullptr); }
void OsxWindowPrivate::OnWindowDidResize() {
  osx_window_->resize_event_.Raise(osx_window_->GetClientSize());
}
}

namespace {
inline NSWindowStyleMask CalcWindowStyleMask(bool frame) {
  return frame ? NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                     NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
               : NSWindowStyleMaskBorderless;
}

}

OsxWindow::OsxWindow(OsxUiApplication* ui_application, INativeWindow* parent, bool frame)
    : OsxGuiResource(ui_application), p_(new details::OsxWindowPrivate(this)) {
  p_->parent_ = parent;

  p_->frame_ = frame;
  p_->content_rect_ = {100, 100, 400, 200};
}

OsxWindow::~OsxWindow() {
  if (p_->window_) {
    [p_->window_ close];
  }

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

std::unique_ptr<graphics::IPainter> OsxWindow::BeginPaint() {
  NSGraphicsContext* ns_graphics_context =
      [NSGraphicsContext graphicsContextWithWindow:p_->window_];

  CGContextRef cg_context = [ns_graphics_context CGContext];

  return std::make_unique<cru::platform::graphics::osx::quartz::QuartzCGContextPainter>(
      GetUiApplication()->GetGraphicsFactory(), cg_context, true, GetClientSize());
}

void OsxWindow::CreateWindow() {
  NSRect content_rect{p_->content_rect_.left, p_->content_rect_.top, p_->content_rect_.width,
                      p_->content_rect_.height};

  NSWindowStyleMask style_mask = CalcWindowStyleMask(p_->frame_);

  p_->window_ = [[NSWindow alloc] initWithContentRect:content_rect
                                            styleMask:style_mask
                                              backing:NSBackingStoreBuffered
                                                defer:false];

  [p_->window_ setDelegate:p_->window_delegate_];

  [p_->window_
      trackEventsMatchingMask:NSEventMaskAny
                      timeout:std::numeric_limits<double>::max()
                         mode:NSRunLoopCommonModes
                      handler:^(NSEvent* _Nullable event, BOOL* _Nonnull stop) {
                        KeyModifier key_modifer;
                        if (event.modifierFlags & NSEventModifierFlagControl)
                          key_modifer |= KeyModifiers::ctrl;
                        if (event.modifierFlags & NSEventModifierFlagOption)
                          key_modifer |= KeyModifiers::alt;
                        if (event.modifierFlags & NSEventModifierFlagShift)
                          key_modifer |= KeyModifiers::shift;

                        switch (event.type) {
                          case NSEventTypeMouseEntered:
                            this->mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Enter);
                            break;
                          case NSEventTypeMouseExited:
                            this->mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Leave);
                            break;
                          case NSEventTypeMouseMoved:
                            this->mouse_move_event_.Raise(
                                Point(event.locationInWindow.x, event.locationInWindow.y));
                            break;
                          case NSEventTypeLeftMouseDown:
                            this->mouse_down_event_.Raise(NativeMouseButtonEventArgs{
                                mouse_buttons::left,
                                Point(event.locationInWindow.x, event.locationInWindow.y),
                                key_modifer});
                            break;
                          case NSEventTypeLeftMouseUp:
                            this->mouse_up_event_.Raise(NativeMouseButtonEventArgs{
                                mouse_buttons::left,
                                Point(event.locationInWindow.x, event.locationInWindow.y),
                                key_modifer});
                            break;
                          case NSEventTypeRightMouseDown:
                            this->mouse_down_event_.Raise(NativeMouseButtonEventArgs{
                                mouse_buttons::right,
                                Point(event.locationInWindow.x, event.locationInWindow.y),
                                key_modifer});
                            break;
                          case NSEventTypeRightMouseUp:
                            this->mouse_up_event_.Raise(NativeMouseButtonEventArgs{
                                mouse_buttons::right,
                                Point(event.locationInWindow.x, event.locationInWindow.y),
                                key_modifer});
                            break;
                          case NSEventTypeScrollWheel:
                            this->mouse_wheel_event_.Raise(NativeMouseWheelEventArgs{
                                static_cast<float>(event.scrollingDeltaY),
                                Point(event.locationInWindow.x, event.locationInWindow.y),
                                key_modifer});
                            break;
                          case NSEventTypeKeyDown:
                            this->key_down_event_.Raise(NativeKeyEventArgs{
                                KeyCodeFromOsxToCru(event.keyCode), key_modifer});
                            break;
                          case NSEventTypeKeyUp:
                            this->key_up_event_.Raise(NativeKeyEventArgs{
                                KeyCodeFromOsxToCru(event.keyCode), key_modifer});
                            break;
                          default:
                            break;
                        }
                        *stop = false;
                      }];
}

namespace details {
class OsxInputMethodContextPrivate {
  friend OsxInputMethodContext;

 public:
  explicit OsxInputMethodContextPrivate(OsxInputMethodContext* input_method_context);

  CRU_DELETE_COPY(OsxInputMethodContextPrivate)
  CRU_DELETE_MOVE(OsxInputMethodContextPrivate)

  ~OsxInputMethodContextPrivate() = default;

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

 private:
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

void OsxInputMethodContextPrivate::RaiseCompositionStartEvent() {
  composition_start_event_.Raise(nullptr);
}
void OsxInputMethodContextPrivate::RaiseCompositionEndEvent() {
  composition_end_event_.Raise(nullptr);
}
void OsxInputMethodContextPrivate::RaiseCompositionEvent() { composition_event_.Raise(nullptr); }

void OsxInputMethodContextPrivate::RaiseTextEvent(StringView text) { text_event_.Raise(text); }
}

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

@implementation WindowDelegate
cru::platform::gui::osx::details::OsxWindowPrivate* p_;

- (id)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p {
  p_ = p;
  return self;
}

- (void)windowWillClose:(NSNotification*)notification {
  p_->OnWindowWillClose();
}

- (void)windowDidExpose:(NSNotification*)notification {
  p_->OnWindowDidExpose();
}

- (void)windowDidUpdate:(NSNotification*)notification {
  p_->OnWindowDidUpdate();
}

- (void)windowDidResize:(NSNotification*)notification {
  p_->OnWindowDidResize();
}
@end

@implementation InputClient
cru::platform::gui::osx::details::OsxInputMethodContextPrivate* _p;
NSMutableAttributedString* _text;

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
  // TODO: Call with window.
}
@end
