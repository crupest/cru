#include "cru/ui/window.hpp"

#include "cru/platform/graph/painter.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/platform/native/window.hpp"
#include "cru/ui/render/window_render_object.hpp"
#include "routed_event_dispatch.hpp"

#include <cassert>
#include <list>

namespace cru::ui {
using cru::platform::native::FocusChangeType;
using cru::platform::native::INativeWindow;
using cru::platform::native::INativeWindowResolver;
using cru::platform::native::IUiApplication;
using cru::platform::native::MouseEnterLeaveType;

namespace event_names {
#ifdef CRU_DEBUG
#define CRU_DEFINE_EVENT_NAME(name) constexpr const char* name = #name;
#else
#define CRU_DEFINE_EVENT_NAME(name) constexpr const char* name = nullptr;
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

Window* Window::CreateOverlapped() {
  return new Window(tag_overlapped_constructor{});
}

namespace {
template <typename T>
inline void BindNativeEvent(
    Window* window, INativeWindow* native_window, IEvent<T>* event,
    void (Window::*handler)(INativeWindow*, typename IEvent<T>::EventArgs),
    std::vector<EventRevokerGuard>& guard_pool) {
  guard_pool.push_back(EventRevokerGuard(event->AddHandler(
      std::bind(handler, window, native_window, std::placeholders::_1))));
}
}  // namespace

Window::Window(tag_overlapped_constructor)
    : mouse_hover_control_(nullptr),
      focus_control_(this),
      mouse_captured_control_(nullptr) {
  window_ = this;
  native_window_resolver_ =
      IUiApplication::GetInstance()->CreateWindow(nullptr);

  const auto native_window = native_window_resolver_->Resolve();

  render_object_.reset(new render::WindowRenderObject(this));
  render_object_->SetAttachedControl(this);

  BindNativeEvent(this, native_window, native_window->DestroyEvent(),
                  &Window::OnNativeDestroy, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->PaintEvent(),
                  &Window::OnNativePaint, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->ResizeEvent(),
                  &Window::OnNativeResize, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->FocusEvent(),
                  &Window::OnNativeFocus, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseEnterLeaveEvent(),
                  &Window::OnNativeMouseEnterLeave, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseMoveEvent(),
                  &Window::OnNativeMouseMove, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseDownEvent(),
                  &Window::OnNativeMouseDown, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseUpEvent(),
                  &Window::OnNativeMouseUp, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->KeyDownEvent(),
                  &Window::OnNativeKeyDown, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->KeyUpEvent(),
                  &Window::OnNativeKeyUp, event_revoker_guards_);
}

Window::~Window() {
  TraverseDescendants(
      [this](Control* control) { control->OnDetachToWindow(this); });
}

std::string_view Window::GetControlType() const { return control_type; }

render::RenderObject* Window::GetRenderObject() const {
  return render_object_.get();
}

platform::native::INativeWindow* Window::GetNativeWindow() {
  return native_window_resolver_->Resolve();
}

bool Window::RequestFocusFor(Control* control) {
  assert(control != nullptr);  // The control to request focus can't be null.
                               // You can set it as the window.

  if (focus_control_ == control) return true;

  DispatchEvent(event_names::LoseFocus, focus_control_,
                &Control::LoseFocusEvent, nullptr, false);

  focus_control_ = control;

  DispatchEvent(event_names::GainFocus, control, &Control::GainFocusEvent,
                nullptr, false);

  return true;
}

Control* Window::GetFocusControl() { return focus_control_; }

bool Window::CaptureMouseFor(Control* control) {
  if (control == mouse_captured_control_) return true;

  if (control == nullptr) {
    const auto old_capture_control = mouse_captured_control_;
    mouse_captured_control_ =
        nullptr;  // update this in case this is used in event handlers
    if (old_capture_control != mouse_hover_control_) {
      DispatchMouseHoverControlChangeEvent(
          old_capture_control, mouse_hover_control_,
          GetNativeWindow()->GetMousePosition(), true, false);
    }
    UpdateCursor();
    return true;
  }

  if (mouse_captured_control_) return false;

  mouse_captured_control_ = control;
  DispatchMouseHoverControlChangeEvent(
      mouse_hover_control_, mouse_captured_control_,
      GetNativeWindow()->GetMousePosition(), false, true);
  UpdateCursor();
  return true;
}

Control* Window::GetMouseCaptureControl() { return mouse_captured_control_; }

void Window::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child) render_object_->RemoveChild(0);
  if (new_child) render_object_->AddChild(new_child->GetRenderObject(), 0);
}

Control* Window::HitTest(const Point& point) {
  return render_object_->HitTest(point)->GetAttachedControl();
}

void Window::OnNativeDestroy(INativeWindow* window, std::nullptr_t) {
  CRU_UNUSED(window)
  delete this;
}

void Window::OnNativePaint(INativeWindow* window, std::nullptr_t) {
  auto painter = window->BeginPaint();
  render_object_->Draw(painter.get());
  painter->EndDraw();
}

void Window::OnNativeResize(INativeWindow* window, const Size& size) {
  CRU_UNUSED(window)
  CRU_UNUSED(size)

  render_object_->GetRenderHost()->InvalidateLayout();
}

void Window::OnNativeFocus(INativeWindow* window, FocusChangeType focus) {
  CRU_UNUSED(window)

  focus == FocusChangeType::Gain
      ? DispatchEvent(event_names::GainFocus, focus_control_,
                      &Control::GainFocusEvent, nullptr, true)
      : DispatchEvent(event_names::LoseFocus, focus_control_,
                      &Control::LoseFocusEvent, nullptr, true);
}

void Window::OnNativeMouseEnterLeave(INativeWindow* window,
                                     MouseEnterLeaveType type) {
  CRU_UNUSED(window)

  if (type == MouseEnterLeaveType::Leave) {
    DispatchEvent(event_names::MouseLeave, mouse_hover_control_,
                  &Control::MouseLeaveEvent, nullptr);
    mouse_hover_control_ = nullptr;
  }
}

void Window::OnNativeMouseMove(INativeWindow* window, const Point& point) {
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

void Window::OnNativeMouseDown(
    INativeWindow* window,
    const platform::native::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(event_names::MouseDown, control, &Control::MouseDownEvent,
                nullptr, args.point, args.button);
}

void Window::OnNativeMouseUp(
    INativeWindow* window,
    const platform::native::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(event_names::MouseUp, control, &Control::MouseUpEvent, nullptr,
                args.point, args.button);
}

void Window::OnNativeKeyDown(INativeWindow* window, int virtual_code) {
  CRU_UNUSED(window)

  DispatchEvent(event_names::KeyDown, focus_control_, &Control::KeyDownEvent,
                nullptr, virtual_code);
}

void Window::OnNativeKeyUp(INativeWindow* window, int virtual_code) {
  CRU_UNUSED(window)

  DispatchEvent(event_names::KeyUp, focus_control_, &Control::KeyUpEvent,
                nullptr, virtual_code);
}

void Window::DispatchMouseHoverControlChangeEvent(Control* old_control,
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

void Window::UpdateCursor() {
  const auto capture = GetMouseCaptureControl();
  GetNativeWindow()->SetCursor(
      (capture ? capture : GetMouseHoverControl())->GetInheritedCursor());
}
}  // namespace cru::ui
