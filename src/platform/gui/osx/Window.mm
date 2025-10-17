#include "cru/platform/gui/osx/Window.h"
#include "WindowPrivate.h"

#include "CursorPrivate.h"
#include "InputMethodPrivate.h"
#include "cru/base/Osx.h"
#include "cru/base/Range.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/Check.h"
#include "cru/platform/graphics/NullPainter.h"
#include "cru/platform/graphics/quartz/Convert.h"
#include "cru/platform/graphics/quartz/Painter.h"
#include "cru/platform/gui/Input.h"
#include "cru/platform/gui/TimerHelper.h"
#include "cru/platform/gui/osx/Cursor.h"
#include "cru/platform/gui/osx/InputMethod.h"
#include "cru/platform/gui/osx/Keyboard.h"
#include "cru/platform/gui/osx/Resource.h"
#include "cru/platform/gui/osx/UiApplication.h"

#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>

#include <limits>
#include <memory>
#include <unordered_set>

namespace {
constexpr int key_down_debug = 0;
}

using ::cru::FromCFStringRef;
using cru::platform::graphics::quartz::Convert;
namespace cru::platform::gui::osx {
namespace {
inline NSWindowStyleMask CalcWindowStyleMask(WindowStyleFlag flag) {
  return flag & WindowStyleFlags::NoCaptionAndBorder
             ? NSWindowStyleMaskBorderless
             : NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                   NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
}
}  // namespace

namespace details {
OsxWindowPrivate::OsxWindowPrivate(OsxWindow* osx_window) : osx_window_(osx_window) {
  window_delegate_ = [[CruWindowDelegate alloc] init:this];

  content_rect_ = {100, 100, 400, 200};

  input_method_context_ = std::make_unique<OsxInputMethodContext>(osx_window);
}

OsxWindowPrivate::~OsxWindowPrivate() {}

void OsxWindowPrivate::OnWindowWillClose() {
  if (window_) destroy_event_.Raise(nullptr);
  window_ = nil;
  CGLayerRelease(draw_layer_);
  draw_layer_ = nullptr;

  if (osx_window_->GetUiApplication()->IsQuitOnAllWindowClosed()) {
    const auto& all_window = osx_window_->GetUiApplication()->GetAllWindow();

    bool quit = true;

    for (auto window : all_window) {
      auto w = CheckPlatform<OsxWindow>(window, osx_window_->GetPlatformId());
      if (w->p_->window_) {
        quit = false;
        break;
      }
    }

    if (quit) {
      osx_window_->GetUiApplication()->RequestQuit(0);
    }
  }
}

void OsxWindowPrivate::OnWindowDidExpose() { osx_window_->RequestRepaint(); }
void OsxWindowPrivate::OnWindowDidUpdate() {}
void OsxWindowPrivate::OnWindowDidMove() { content_rect_ = RetrieveContentRect(); }

void OsxWindowPrivate::OnWindowDidResize() {
  content_rect_ = RetrieveContentRect();

  auto view = [window_ contentView];
  [view removeTrackingArea:[view trackingAreas][0]];
  auto tracking_area = [[NSTrackingArea alloc]
      initWithRect:CGRectMake(0, 0, content_rect_.width, content_rect_.height)
           options:(NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways)
             owner:view
          userInfo:nil];
  [view addTrackingArea:tracking_area];

  CGLayerRelease(draw_layer_);
  draw_layer_ = CreateLayer(Convert(content_rect_.GetSize()));

  resize_event_.Raise(osx_window_->GetClientSize());

  osx_window_->RequestRepaint();
}

void OsxWindowPrivate::OnBecomeKeyWindow() { focus_event_.Raise(FocusChangeType::Gain); }

void OsxWindowPrivate::OnResignKeyWindow() { focus_event_.Raise(FocusChangeType::Lose); }

void OsxWindowPrivate::OnMouseEnterLeave(MouseEnterLeaveType type) {
  mouse_enter_leave_event_.Raise(type);
  if (type == MouseEnterLeaveType::Enter) {
    mouse_in_ = true;
    UpdateCursor();
  } else {
    mouse_in_ = false;
  }
}

void OsxWindowPrivate::OnMouseMove(Point p) { mouse_move_event_.Raise(TransformMousePoint(p)); }

void OsxWindowPrivate::OnMouseDown(MouseButton button, Point p, KeyModifier key_modifier) {
  mouse_down_event_.Raise({button, TransformMousePoint(p), key_modifier});
}

void OsxWindowPrivate::OnMouseUp(MouseButton button, Point p, KeyModifier key_modifier) {
  mouse_up_event_.Raise({button, TransformMousePoint(p), key_modifier});
}

void OsxWindowPrivate::OnMouseWheel(float delta, Point p, KeyModifier key_modifier,
                                    bool horizontal) {
  mouse_wheel_event_.Raise({delta, TransformMousePoint(p), key_modifier, horizontal});
}

void OsxWindowPrivate::OnKeyDown(KeyCode key, KeyModifier key_modifier) {
  key_down_event_.Raise({key, key_modifier});
}

void OsxWindowPrivate::OnKeyUp(KeyCode key, KeyModifier key_modifier) {
  key_up_event_.Raise({key, key_modifier});
}

CGLayerRef OsxWindowPrivate::CreateLayer(const CGSize& size) {
  auto s = size;
  if (s.width == 0) s.width = 1;
  if (s.height == 0) s.height = 1;

  auto draw_layer = CGLayerCreateWithContext(nullptr, s, nullptr);
  Ensures(draw_layer);

  return draw_layer;
}

void OsxWindowPrivate::UpdateCursor() {
  auto cursor = cursor_ == nullptr
                    ? std::dynamic_pointer_cast<OsxCursor>(
                          osx_window_->GetUiApplication()->GetCursorManager()->GetSystemCursor(
                              SystemCursorType::Arrow))
                    : cursor_;

  [cursor->p_->ns_cursor_ set];
}

Point OsxWindowPrivate::TransformMousePoint(const Point& point) {
  Point r = point;
  r.y = content_rect_.height - r.y;
  return r;
}

void OsxWindowPrivate::CreateWindow() {
  Expects(!window_);

  NSWindowStyleMask style_mask = CalcWindowStyleMask(style_flag_);
  window_ = [[CruWindow alloc] init:this
                        contentRect:{0, 0, content_rect_.width, content_rect_.height}
                              style:style_mask];
  Ensures(window_);

  osx_window_->SetClientRect(content_rect_);

  [window_ setDelegate:window_delegate_];

  if (parent_) {
    auto parent = CheckPlatform<OsxWindow>(parent_, this->osx_window_->GetPlatformId());
    [window_ setParentWindow:parent->p_->window_];
  }

  NSView* content_view = [[CruView alloc] init:this
                               input_context_p:input_method_context_->p_.get()
                                         frame:Rect(Point{}, content_rect_.GetSize())];

  [window_ setContentView:content_view];
  [window_ setTitle:(NSString*)ToCFString(title_).ref];

  draw_layer_ = CreateLayer(Convert(content_rect_.GetSize()));

  create_event_.Raise(nullptr);

  osx_window_->RequestRepaint();
}

Size OsxWindowPrivate::GetScreenSize() {
  auto screen = window_ ? [window_ screen] : [NSScreen mainScreen];
  auto size = [screen frame].size;
  return Convert(size);
}

Rect OsxWindowPrivate::RetrieveContentRect() {
  NSRect rect = [NSWindow contentRectForFrameRect:[window_ frame]
                                        styleMask:CalcWindowStyleMask(style_flag_)];
  rect.origin.y = GetScreenSize().height - rect.origin.y - rect.size.height;
  return cru::platform::graphics::quartz::Convert(rect);
}

}  // namespace details

OsxWindow::OsxWindow(OsxUiApplication* ui_application)
    : OsxGuiResource(ui_application), p_(new details::OsxWindowPrivate(this)) {}

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

void OsxWindow::SetParent(INativeWindow* parent) {
  auto p = CheckPlatform<OsxWindow>(parent, GetPlatformId());

  p_->parent_ = parent;

  if (p_->window_) {
    [p_->window_ setParentWindow:p->p_->window_];
  }
}

WindowStyleFlag OsxWindow::GetStyleFlag() { return p_->style_flag_; }

void OsxWindow::SetStyleFlag(WindowStyleFlag flag) {
  p_->style_flag_ = flag;

  if (p_->window_) {
    [p_->window_ close];
  }
}

std::string OsxWindow::GetTitle() { return p_->title_; }

void OsxWindow::SetTitle(std::string title) {
  p_->title_ = title;

  if (p_->window_) {
    [p_->window_ setTitle:(NSString*)ToCFString(title).ref];
  }
}

WindowVisibilityType OsxWindow::GetVisibility() {
  if (!p_->window_) return WindowVisibilityType::Hide;
  if ([p_->window_ isMiniaturized]) return WindowVisibilityType::Minimize;
  return [p_->window_ isVisible] ? WindowVisibilityType::Show : WindowVisibilityType::Hide;
}

void OsxWindow::SetVisibility(WindowVisibilityType visibility) {
  if (p_->window_) {
    if (visibility == WindowVisibilityType::Show) {
      [p_->window_ orderFront:nil];
      p_->visibility_change_event_.Raise(WindowVisibilityType::Show);
    } else if (visibility == WindowVisibilityType::Hide) {
      [p_->window_ orderOut:nil];
      p_->visibility_change_event_.Raise(WindowVisibilityType::Hide);
    } else if (visibility == WindowVisibilityType::Minimize) {
      [p_->window_ miniaturize:nil];
    }
  } else {
    if (visibility == WindowVisibilityType::Show) {
      p_->CreateWindow();
      [p_->window_ orderFront:nil];
      p_->visibility_change_event_.Raise(WindowVisibilityType::Show);
    }
  }
}

Size OsxWindow::GetClientSize() { return p_->content_rect_.GetSize(); }

void OsxWindow::SetClientSize(const Size& size) {
  if (p_->window_) {
    auto rect = GetClientRect();
    rect.SetSize(size);
    SetClientRect(rect);
  } else {
    p_->content_rect_.SetSize(size);
  }
}

Rect OsxWindow::GetClientRect() { return p_->content_rect_; }

void OsxWindow::SetClientRect(const Rect& rect) {
  if (p_->window_) {
    auto r = Convert(rect);
    r.origin.y = p_->GetScreenSize().height - r.origin.y - r.size.height;
    r = [NSWindow frameRectForContentRect:r styleMask:CalcWindowStyleMask(p_->style_flag_)];
    [p_->window_ setFrame:r display:false];
  } else {
    p_->content_rect_ = rect;
  }
}

Rect OsxWindow::GetWindowRect() {
  auto r = Convert(p_->content_rect_);
  r.origin.y = p_->GetScreenSize().height - r.origin.y - r.size.height;
  r = [NSWindow frameRectForContentRect:r styleMask:CalcWindowStyleMask(p_->style_flag_)];
  r.origin.y = p_->GetScreenSize().height - r.origin.y - r.size.height;
  return Convert(r);
}

void OsxWindow::SetWindowRect(const Rect& rect) {
  auto r = Convert(rect);
  r.origin.y = p_->GetScreenSize().height - r.origin.y - r.size.height;
  r = [NSWindow frameRectForContentRect:r styleMask:CalcWindowStyleMask(p_->style_flag_)];
  r.origin.y = p_->GetScreenSize().height - r.origin.y - r.size.height;
  SetClientRect(Convert(r));
}

void OsxWindow::RequestRepaint() {
  if (!p_->draw_timer_) {
    p_->draw_timer_ = GetUiApplication()->SetImmediate([this] {
      p_->paint_event_.Raise(nullptr);
      p_->draw_timer_.Release();
    });
  }
}

std::unique_ptr<graphics::IPainter> OsxWindow::BeginPaint() {
  if (!p_->window_) {
    return std::make_unique<graphics::NullPainter>();
  }

  CGContextRef cg_context = CGLayerGetContext(p_->draw_layer_);

  return std::make_unique<cru::platform::graphics::quartz::QuartzCGContextPainter>(
      GetUiApplication()->GetGraphicsFactory(), cg_context, false, GetClientSize(),
      [this](graphics::quartz::QuartzCGContextPainter*) {
        [[p_->window_ contentView] setNeedsDisplay:YES];
      });
}

bool OsxWindow::RequestFocus() {
  if (!p_->window_) return false;
  [p_->window_ makeKeyWindow];
  return true;
}

Point OsxWindow::GetMousePosition() {
  auto p = [p_->window_ mouseLocationOutsideOfEventStream];
  return Point(p.x, p.y);
}

bool OsxWindow::CaptureMouse() { return true; }

bool OsxWindow::ReleaseMouse() { return true; }

void OsxWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  p_->cursor_ = CheckPlatform<OsxCursor>(cursor, GetPlatformId());
  p_->UpdateCursor();
}

void OsxWindow::SetToForeground() {
  if (!p_->window_) return;
  [p_->window_ makeMainWindow];
  [p_->window_ orderFrontRegardless];
}

IEvent<std::nullptr_t>* OsxWindow::CreateEvent() { return &p_->create_event_; }
IEvent<std::nullptr_t>* OsxWindow::DestroyEvent() { return &p_->destroy_event_; }
IEvent<std::nullptr_t>* OsxWindow::PaintEvent() { return &p_->paint_event_; }
IEvent<WindowVisibilityType>* OsxWindow::VisibilityChangeEvent() {
  return &p_->visibility_change_event_;
}
IEvent<Size>* OsxWindow::ResizeEvent() { return &p_->resize_event_; }
IEvent<FocusChangeType>* OsxWindow::FocusEvent() { return &p_->focus_event_; }
IEvent<MouseEnterLeaveType>* OsxWindow::MouseEnterLeaveEvent() {
  return &p_->mouse_enter_leave_event_;
}
IEvent<Point>* OsxWindow::MouseMoveEvent() { return &p_->mouse_move_event_; }
IEvent<NativeMouseButtonEventArgs>* OsxWindow::MouseDownEvent() { return &p_->mouse_down_event_; }
IEvent<NativeMouseButtonEventArgs>* OsxWindow::MouseUpEvent() { return &p_->mouse_up_event_; }
IEvent<NativeMouseWheelEventArgs>* OsxWindow::MouseWheelEvent() { return &p_->mouse_wheel_event_; }
IEvent<NativeKeyEventArgs>* OsxWindow::KeyDownEvent() { return &p_->key_down_event_; }
IEvent<NativeKeyEventArgs>* OsxWindow::KeyUpEvent() { return &p_->key_up_event_; }

IInputMethodContext* OsxWindow::GetInputMethodContext() { return p_->input_method_context_.get(); }
}  // namespace cru::platform::gui::osx

namespace {
cru::platform::gui::KeyModifier GetKeyModifier(NSEvent* event) {
  cru::platform::gui::KeyModifier key_modifier;
  if (event.modifierFlags & NSEventModifierFlagControl)
    key_modifier |= cru::platform::gui::KeyModifiers::Ctrl;
  if (event.modifierFlags & NSEventModifierFlagOption)
    key_modifier |= cru::platform::gui::KeyModifiers::Alt;
  if (event.modifierFlags & NSEventModifierFlagShift)
    key_modifier |= cru::platform::gui::KeyModifiers::Shift;
  if (event.modifierFlags & NSEventModifierFlagCommand)
    key_modifier |= cru::platform::gui::KeyModifiers::Command;
  return key_modifier;
}
}  // namespace

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

  [self setAcceptsMouseMovedEvents:YES];

  return self;
}

- (BOOL)canBecomeMainWindow {
  return YES;
}

- (BOOL)canBecomeKeyWindow {
  return YES;
}
@end

@implementation CruView {
  cru::platform::gui::osx::details::OsxWindowPrivate* _p;
  cru::platform::gui::osx::details::OsxInputMethodContextPrivate* _input_context_p;
  NSMutableAttributedString* _input_context_text;
}

- (instancetype)init:(cru::platform::gui::osx::details::OsxWindowPrivate*)p
     input_context_p:
         (cru::platform::gui::osx::details::OsxInputMethodContextPrivate*)input_context_p
               frame:(cru::platform::Rect)frame {
  [super initWithFrame:cru::platform::graphics::quartz::Convert(frame)];
  _p = p;
  _input_context_p = input_context_p;

  auto tracking_area = [[NSTrackingArea alloc]
      initWithRect:Convert(frame)
           options:(NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways)
             owner:self
          userInfo:nil];
  [self addTrackingArea:tracking_area];

  return self;
}

- (void)drawRect:(NSRect)dirtyRect {
  auto cg_context = [[NSGraphicsContext currentContext] CGContext];
  auto layer = _p->GetDrawLayer();
  cru::Ensures(layer);
  CGContextDrawLayerAtPoint(cg_context, CGPointMake(0, 0), layer);
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

- (BOOL)canBecomeKeyView {
  return YES;
}

- (void)mouseMoved:(NSEvent*)event {
  _p->OnMouseMove(cru::platform::Point(event.locationInWindow.x, event.locationInWindow.y));
}

- (void)mouseDragged:(NSEvent*)event {
  _p->OnMouseMove(cru::platform::Point(event.locationInWindow.x, event.locationInWindow.y));
}

- (void)rightMouseDragged:(NSEvent*)event {
  _p->OnMouseMove(cru::platform::Point(event.locationInWindow.x, event.locationInWindow.y));
}

- (void)mouseEntered:(NSEvent*)event {
  _p->OnMouseEnterLeave(cru::platform::gui::MouseEnterLeaveType::Enter);
}

- (void)mouseExited:(NSEvent*)event {
  _p->OnMouseEnterLeave(cru::platform::gui::MouseEnterLeaveType::Leave);
}

- (void)mouseDown:(NSEvent*)event {
  [[self window] makeKeyWindow];

  auto key_modifier = GetKeyModifier(event);
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseDown(cru::platform::gui::MouseButtons::Left, p, key_modifier);
}

- (void)mouseUp:(NSEvent*)event {
  auto key_modifier = GetKeyModifier(event);
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseUp(cru::platform::gui::MouseButtons::Left, p, key_modifier);
}

- (void)rightMouseDown:(NSEvent*)event {
  auto key_modifier = GetKeyModifier(event);
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseDown(cru::platform::gui::MouseButtons::Right, p, key_modifier);
}

- (void)rightMouseUp:(NSEvent*)event {
  auto key_modifier = GetKeyModifier(event);
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  _p->OnMouseUp(cru::platform::gui::MouseButtons::Right, p, key_modifier);
}

- (void)scrollWheel:(NSEvent*)event {
  auto key_modifier = GetKeyModifier(event);
  cru::platform::Point p(event.locationInWindow.x, event.locationInWindow.y);

  if (event.scrollingDeltaY) {
    _p->OnMouseWheel(static_cast<float>(event.scrollingDeltaY), p, key_modifier, false);
  }

  if (event.scrollingDeltaX) {
    _p->OnMouseWheel(static_cast<float>(event.scrollingDeltaX), p, key_modifier, true);
  }
}

namespace {
using cru::platform::gui::KeyCode;
const std::unordered_set<KeyCode> input_context_handle_codes{
    KeyCode::A,
    KeyCode::B,
    KeyCode::C,
    KeyCode::D,
    KeyCode::E,
    KeyCode::F,
    KeyCode::G,
    KeyCode::H,
    KeyCode::I,
    KeyCode::J,
    KeyCode::K,
    KeyCode::L,
    KeyCode::M,
    KeyCode::N,
    KeyCode::O,
    KeyCode::P,
    KeyCode::Q,
    KeyCode::R,
    KeyCode::S,
    KeyCode::T,
    KeyCode::U,
    KeyCode::V,
    KeyCode::W,
    KeyCode::X,
    KeyCode::Y,
    KeyCode::Z,
    KeyCode::N0,
    KeyCode::N1,
    KeyCode::N2,
    KeyCode::N3,
    KeyCode::N4,
    KeyCode::N5,
    KeyCode::N6,
    KeyCode::N7,
    KeyCode::N8,
    KeyCode::N9,
    KeyCode::Comma,
    KeyCode::Period,
    KeyCode::Slash,
    KeyCode::Semicolon,
    KeyCode::Quote,
    KeyCode::LeftSquareBracket,
    KeyCode::RightSquareBracket,
    KeyCode::BackSlash,
    KeyCode::Minus,
    KeyCode::Equal,
    KeyCode::GraveAccent,
};
}  // namespace

const std::unordered_set<KeyCode> input_context_handle_codes_when_has_text{
    KeyCode::Backspace, KeyCode::Space, KeyCode::Return, KeyCode::Left,
    KeyCode::Right,     KeyCode::Up,    KeyCode::Down};

- (void)keyDown:(NSEvent*)event {
  auto key_modifier = GetKeyModifier(event);

  bool handled = false;

  auto input_context = dynamic_cast<cru::platform::gui::osx::OsxInputMethodContext*>(
      _p->GetWindow()->GetInputMethodContext());
  Ensures(input_context);

  auto c = cru::platform::gui::osx::KeyCodeFromOsxToCru(event.keyCode);

  if (input_context->IsEnabled()) {
    if (input_context_handle_codes.count(c) &&
        !(key_modifier & ~cru::platform::gui::KeyModifiers::Shift)) {
      handled = [[self inputContext] handleEvent:event];
    } else if (input_context_handle_codes_when_has_text.count(c) && !key_modifier) {
      if (!input_context->GetCompositionText().text.empty()) {
        handled = [[self inputContext] handleEvent:event];
      } else {
        if (c == KeyCode::Return) {
          _input_context_p->RaiseTextEvent("\n");
          handled = true;
        } else if (c == KeyCode::Space) {
          _input_context_p->RaiseTextEvent(" ");
          handled = true;
        }
      }
    }
  }

  if (!handled) {
    _p->OnKeyDown(c, key_modifier);
  }
}

- (void)keyUp:(NSEvent*)event {
  // cru::CRU_LOG_TAG_DEBUG(u"CruView", u"Recieved key up.");

  auto key_modifier = GetKeyModifier(event);
  auto c = cru::platform::gui::osx::KeyCodeFromOsxToCru(event.keyCode);

  _p->OnKeyUp(c, key_modifier);
}

- (BOOL)hasMarkedText {
  return _input_context_text != nil;
}

- (NSRange)markedRange {
  return _input_context_text == nil ? NSRange{NSNotFound, 0}
                                    : NSRange{0, [_input_context_text length]};
}

- (NSRange)selectedRange {
  return NSMakeRange(_input_context_p->GetSelectionRange().position,
                     _input_context_p->GetSelectionRange().count);
}

- (void)setMarkedText:(id)string
        selectedRange:(NSRange)selectedRange
     replacementRange:(NSRange)replacementRange {
  CFStringRef s;
  if ([string isKindOfClass:[NSString class]]) {
    s = (CFStringRef)string;
  } else {
    auto as = (CFAttributedStringRef)string;
    s = CFAttributedStringGetString(as);
  }

  auto ss = FromCFStringRef(s);

  // cru::CRU_LOG_TAG_DEBUG(u"CruView",
  //                    u"Received setMarkedText string: {}, selected range: ({}, {}), "
  //                    u"replacement range: ({}, {}).",
  //                    ss, selectedRange.location, selectedRange.length, replacementRange.location,
  //                    replacementRange.length);

  if (_input_context_text == nil) {
    _input_context_text = [[NSMutableAttributedString alloc] init];
    _input_context_p->RaiseCompositionStartEvent();
  }

  if (replacementRange.location == NSNotFound) replacementRange.location = 0;

  [_input_context_text
      replaceCharactersInRange:NSMakeRange(0, [_input_context_text length])
          withAttributedString:[[NSAttributedString alloc] initWithString:(NSString*)s]];

  cru::platform::gui::CompositionText composition_text;
  composition_text.text = FromCFStringRef((CFStringRef)[_input_context_text string]);
  composition_text.selection.position =
      cru::string::Utf8IndexCodePointToCodeUnit(ss.data(), ss.size(), selectedRange.location);
  composition_text.selection.count =
      cru::string::Utf8IndexCodePointToCodeUnit(ss.data(), ss.size(),
                                                selectedRange.location + selectedRange.length) -
      composition_text.selection.position;
  _input_context_p->SetCompositionText(composition_text);
  _input_context_p->RaiseCompositionEvent();
}

- (void)unmarkText {
  _input_context_text = nil;
  _input_context_p->RaiseCompositionEndEvent();
}

- (NSArray<NSAttributedStringKey>*)validAttributesForMarkedText {
  return @[
    (NSString*)kCTUnderlineColorAttributeName, (NSString*)kCTUnderlineStyleAttributeName,
    (NSString*)kCTForegroundColorAttributeName, (NSString*)kCTBackgroundColorAttributeName
  ];
}

- (NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range
                                               actualRange:(NSRangePointer)actualRange {
  cru::Range r(range.location, range.length);

  r = r.CoerceInto(0, [_input_context_text length]);

  return [_input_context_text attributedSubstringFromRange:NSMakeRange(r.position, r.count)];
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {
  CFStringRef s;
  if ([string isKindOfClass:[NSString class]]) {
    s = (CFStringRef)string;
  } else {
    auto as = (CFAttributedStringRef)string;
    s = CFAttributedStringGetString(as);
  }

  _input_context_text = nil;
  _input_context_p->SetCompositionText(cru::platform::gui::CompositionText());
  auto ss = FromCFStringRef(s);

  // cru::CRU_LOG_TAG_DEBUG(u"CruView", u"Finish composition: {}, replacement range: ({}, {})", ss,
  //                    replacementRange.location, replacementRange.length);

  _input_context_p->RaiseTextEvent(ss);
  _input_context_p->RaiseCompositionEvent();
  _input_context_p->RaiseCompositionEndEvent();
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point {
  return NSNotFound;
}

// The key to composition window. It is in screen coordinate.
- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actualRange {
  const auto window_rect = _p->GetWindow()->GetClientRect();
  auto position = _input_context_p->GetCandidateWindowPosition();

  position.x += window_rect.left;
  position.y += window_rect.top;
  position.y = _p->GetScreenSize().height - position.y;

  NSRect result;
  result.origin.x = position.x;
  result.origin.y = position.y;
  result.size.height = 0;
  result.size.width = 0;
  return result;
}

- (void)doCommandBySelector:(SEL)selector {
  _input_context_p->PerformSel(selector);
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

- (void)windowDidMove:(NSNotification*)notification {
  _p->OnWindowDidMove();
}

- (void)windowDidResize:(NSNotification*)notification {
  _p->OnWindowDidResize();
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
  _p->OnBecomeKeyWindow();
}

- (void)windowDidResignKey:(NSNotification*)notification {
  _p->OnResignKeyWindow();
}
@end
