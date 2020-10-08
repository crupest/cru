#include "cru/ui/UiHost.hpp"

#include "RoutedEventDispatch.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/graph/Painter.hpp"
#include "cru/platform/native/InputMethod.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/platform/native/Window.hpp"
#include "cru/ui/Window.hpp"
#include "cru/ui/render/WindowRenderObject.hpp"

namespace cru::ui {
using platform::native::INativeWindow;
using platform::native::IUiApplication;

namespace event_names {
#ifdef CRU_DEBUG
// clang-format off
#define CRU_DEFINE_EVENT_NAME(name) constexpr const char16_t* name = CRU_MAKE_UNICODE_LITERAL(name);
// clang-format on
#else
#define CRU_DEFINE_EVENT_NAME(name) constexpr const char16_t* name = u"";
#endif

CRU_DEFINE_EVENT_NAME(LoseFocus)
CRU_DEFINE_EVENT_NAME(GainFocus)
CRU_DEFINE_EVENT_NAME(MouseEnter)
CRU_DEFINE_EVENT_NAME(MouseLeave)
CRU_DEFINE_EVENT_NAME(MouseMove)
CRU_DEFINE_EVENT_NAME(MouseDown)
CRU_DEFINE_EVENT_NAME(MouseUp)
CRU_DEFINE_EVENT_NAME(KeyDown)
CRU_DEFINE_EVENT_NAME(KeyUp)

#undef CRU_DEFINE_EVENT_NAME
}  // namespace event_names

namespace {
bool IsAncestor(Control* control, Control* ancestor) {
  while (control != nullptr) {
    if (control == ancestor) return true;
    control = control->GetParent();
  }
  return false;
}

std::list<Control*> GetAncestorList(Control* control) {
  std::list<Control*> l;
  while (control != nullptr) {
    l.push_front(control);
    control = control->GetParent();
  }
  return l;
}

Control* FindLowestCommonAncestor(Control* left, Control* right) {
  if (left == nullptr || right == nullptr) return nullptr;

  auto&& left_list = GetAncestorList(left);
  auto&& right_list = GetAncestorList(right);

  // the root is different
  if (left_list.front() != right_list.front()) return nullptr;

  // find the last same control or the last control (one is ancestor of the
  // other)
  auto left_i = left_list.cbegin();
  auto right_i = right_list.cbegin();

  while (true) {
    if (left_i == left_list.cend()) {
      return *(--left_i);
    }
    if (right_i == right_list.cend()) {
      return *(--right_i);
    }
    if (*left_i != *right_i) {
      return *(--left_i);
    }
    ++left_i;
    ++right_i;
  }
}
}  // namespace

namespace {
template <typename T>
inline void BindNativeEvent(
    UiHost* host, INativeWindow* native_window, IEvent<T>* event,
    void (UiHost::*handler)(INativeWindow*, typename IEvent<T>::EventArgs),
    std::vector<EventRevokerGuard>& guard_pool) {
  guard_pool.push_back(EventRevokerGuard(event->AddHandler(
      std::bind(handler, host, native_window, std::placeholders::_1))));
}
}  // namespace

UiHost::UiHost(Window* window)
    : window_control_(window),
      mouse_hover_control_(nullptr),
      focus_control_(window),
      mouse_captured_control_(nullptr) {
  const auto ui_application = IUiApplication::GetInstance();
  native_window_resolver_ = ui_application->CreateWindow(nullptr);

  const auto native_window = native_window_resolver_->Resolve();

  auto input_method_context =
      ui_application->GetInputMethodManager()->GetContext(native_window);
  input_method_context->DisableIME();

  window->ui_host_ = this;

  root_render_object_ = std::make_unique<render::WindowRenderObject>(this);
  root_render_object_->SetAttachedControl(window);
  window->render_object_ = root_render_object_.get();

  BindNativeEvent(this, native_window, native_window->DestroyEvent(),
                  &UiHost::OnNativeDestroy, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->PaintEvent(),
                  &UiHost::OnNativePaint, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->ResizeEvent(),
                  &UiHost::OnNativeResize, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->FocusEvent(),
                  &UiHost::OnNativeFocus, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseEnterLeaveEvent(),
                  &UiHost::OnNativeMouseEnterLeave, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseMoveEvent(),
                  &UiHost::OnNativeMouseMove, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseDownEvent(),
                  &UiHost::OnNativeMouseDown, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseUpEvent(),
                  &UiHost::OnNativeMouseUp, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->KeyDownEvent(),
                  &UiHost::OnNativeKeyDown, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->KeyUpEvent(),
                  &UiHost::OnNativeKeyUp, event_revoker_guards_);
}

UiHost::~UiHost() {
  deleting_ = true;
  window_control_->TraverseDescendants(
      [this](Control* control) { control->OnDetachFromHost(this); });
  if (!native_window_destroyed_) {
    const auto native_window = native_window_resolver_->Resolve();
    if (native_window) {
      native_window->Close();
    }
  }
}

void UiHost::InvalidatePaint() {
  if (const auto native_window = native_window_resolver_->Resolve())
    native_window->RequestRepaint();
}

void UiHost::InvalidateLayout() {
  log::TagDebug(log_tag, u"A relayout is requested.");
  if (!need_layout_) {
    platform::native::IUiApplication::GetInstance()->InvokeLater(
        [resolver = this->CreateResolver()] {
          if (const auto host = resolver.Resolve()) {
            host->Relayout();
            host->need_layout_ = false;
            host->InvalidatePaint();
          }
        });
    need_layout_ = true;
  }
}

void UiHost::Relayout() {
  const auto native_window = native_window_resolver_->Resolve();
  const auto client_size = native_window
                               ? native_window->GetClientSize()
                               : Size{100, 100};  // a reasonable assumed size
  root_render_object_->Measure(
      render::MeasureRequirement{client_size,
                                 render::MeasureSize::NotSpecified()},
      render::MeasureSize::NotSpecified());
  root_render_object_->Layout(Point{});
  after_layout_event_.Raise(AfterLayoutEventArgs{});
  log::TagDebug(log_tag, u"A relayout is finished.");
}

bool UiHost::RequestFocusFor(Control* control) {
  Expects(control != nullptr);  // The control to request focus can't be null.
                                // You can set it as the window.

  if (focus_control_ == control) return true;

  const auto old_focus_control = focus_control_;

  focus_control_ = control;

  DispatchEvent(event_names::LoseFocus, old_focus_control,
                &Control::LoseFocusEvent, nullptr, false);

  DispatchEvent(event_names::GainFocus, control, &Control::GainFocusEvent,
                nullptr, false);

  return true;
}

Control* UiHost::GetFocusControl() { return focus_control_; }

bool UiHost::CaptureMouseFor(Control* control) {
  const auto native_window = native_window_resolver_->Resolve();
  if (!native_window) return false;

  if (control == mouse_captured_control_) return true;

  if (control == nullptr) {
    const auto old_capture_control = mouse_captured_control_;
    mouse_captured_control_ =
        nullptr;  // update this in case this is used in event handlers
    if (old_capture_control != mouse_hover_control_) {
      DispatchMouseHoverControlChangeEvent(
          old_capture_control, mouse_hover_control_,
          native_window->GetMousePosition(), true, false);
    }
    UpdateCursor();
    return true;
  }

  if (mouse_captured_control_) return false;

  mouse_captured_control_ = control;
  DispatchMouseHoverControlChangeEvent(
      mouse_hover_control_, mouse_captured_control_,
      native_window->GetMousePosition(), false, true);
  UpdateCursor();
  return true;
}

Control* UiHost::GetMouseCaptureControl() { return mouse_captured_control_; }

void UiHost::OnNativeDestroy(INativeWindow* window, std::nullptr_t) {
  CRU_UNUSED(window)
  native_window_destroyed_ = true;
  if (!deleting_ && !retain_after_destroy_) delete window_control_;
}

void UiHost::OnNativePaint(INativeWindow* window, std::nullptr_t) {
  auto painter = window->BeginPaint();
  painter->Clear(colors::white);
  root_render_object_->Draw(painter.get());
  painter->EndDraw();
}

void UiHost::OnNativeResize(INativeWindow* window, const Size& size) {
  CRU_UNUSED(window)
  CRU_UNUSED(size)

  InvalidateLayout();
}

void UiHost::OnNativeFocus(INativeWindow* window,
                           platform::native::FocusChangeType focus) {
  CRU_UNUSED(window)

  focus == platform::native::FocusChangeType::Gain
      ? DispatchEvent(event_names::GainFocus, focus_control_,
                      &Control::GainFocusEvent, nullptr, true)
      : DispatchEvent(event_names::LoseFocus, focus_control_,
                      &Control::LoseFocusEvent, nullptr, true);
}

void UiHost::OnNativeMouseEnterLeave(
    INativeWindow* window, platform::native::MouseEnterLeaveType type) {
  CRU_UNUSED(window)

  if (type == platform::native::MouseEnterLeaveType::Leave) {
    DispatchEvent(event_names::MouseLeave, mouse_hover_control_,
                  &Control::MouseLeaveEvent, nullptr);
    mouse_hover_control_ = nullptr;
  }
}

void UiHost::OnNativeMouseMove(INativeWindow* window, const Point& point) {
  CRU_UNUSED(window)

  // Find the first control that hit test succeed.
  const auto new_mouse_hover_control = HitTest(point);
  const auto old_mouse_hover_control = mouse_hover_control_;
  mouse_hover_control_ = new_mouse_hover_control;

  if (mouse_captured_control_) {
    const auto n = FindLowestCommonAncestor(new_mouse_hover_control,
                                            mouse_captured_control_);
    const auto o = FindLowestCommonAncestor(old_mouse_hover_control,
                                            mouse_captured_control_);
    bool a = IsAncestor(o, n);
    if (a) {
      DispatchEvent(event_names::MouseLeave, o, &Control::MouseLeaveEvent, n);
    } else {
      DispatchEvent(event_names::MouseEnter, n, &Control::MouseEnterEvent, o,
                    point);
    }
    DispatchEvent(event_names::MouseMove, mouse_captured_control_,
                  &Control::MouseMoveEvent, nullptr, point);
    UpdateCursor();
    return;
  }

  DispatchMouseHoverControlChangeEvent(
      old_mouse_hover_control, new_mouse_hover_control, point, false, false);
  DispatchEvent(event_names::MouseMove, new_mouse_hover_control,
                &Control::MouseMoveEvent, nullptr, point);
  UpdateCursor();
}

void UiHost::OnNativeMouseDown(
    INativeWindow* window,
    const platform::native::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(event_names::MouseDown, control, &Control::MouseDownEvent,
                nullptr, args.point, args.button, args.modifier);
}

void UiHost::OnNativeMouseUp(
    INativeWindow* window,
    const platform::native::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(event_names::MouseUp, control, &Control::MouseUpEvent, nullptr,
                args.point, args.button, args.modifier);
}

void UiHost::OnNativeKeyDown(INativeWindow* window,
                             const platform::native::NativeKeyEventArgs& args) {
  CRU_UNUSED(window)

  DispatchEvent(event_names::KeyDown, focus_control_, &Control::KeyDownEvent,
                nullptr, args.key, args.modifier);
}

void UiHost::OnNativeKeyUp(INativeWindow* window,
                           const platform::native::NativeKeyEventArgs& args) {
  CRU_UNUSED(window)

  DispatchEvent(event_names::KeyUp, focus_control_, &Control::KeyUpEvent,
                nullptr, args.key, args.modifier);
}

void UiHost::DispatchMouseHoverControlChangeEvent(Control* old_control,
                                                  Control* new_control,
                                                  const Point& point,
                                                  bool no_leave,
                                                  bool no_enter) {
  if (new_control != old_control)  // if the mouse-hover-on control changed
  {
    const auto lowest_common_ancestor =
        FindLowestCommonAncestor(old_control, new_control);
    if (!no_leave && old_control != nullptr)
      DispatchEvent(event_names::MouseLeave, old_control,
                    &Control::MouseLeaveEvent,
                    lowest_common_ancestor);  // dispatch mouse leave event.
    if (!no_enter && new_control != nullptr) {
      DispatchEvent(event_names::MouseEnter, new_control,
                    &Control::MouseEnterEvent, lowest_common_ancestor,
                    point);  // dispatch mouse enter event.
    }
  }
}

void UiHost::UpdateCursor() {
  if (const auto native_window = native_window_resolver_->Resolve()) {
    const auto capture = GetMouseCaptureControl();
    native_window->SetCursor(
        (capture ? capture : GetMouseHoverControl())->GetInheritedCursor());
  }
}

Control* UiHost::HitTest(const Point& point) {
  const auto render_object = root_render_object_->HitTest(point);
  if (render_object) {
    const auto control = render_object->GetAttachedControl();
    Ensures(control);
    return control;
  }
  return window_control_;
}
}  // namespace cru::ui
