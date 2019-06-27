#include "cru/ui/window.hpp"

#include "cru/platform/graph/painter.hpp"
#include "cru/platform/native/native_window.hpp"
#include "cru/platform/native/ui_applicaition.hpp"
#include "cru/ui/render/window_render_object.hpp"

#include <cassert>

namespace cru::ui {
namespace {
// Dispatch the event.
//
// This will raise routed event of the control and its parent and parent's
// parent ... (until "last_receiver" if it's not nullptr) with appropriate args.
//
// First tunnel from top to bottom possibly stopped by "handled" flag in
// EventArgs. Second bubble from bottom to top possibly stopped by "handled"
// flag in EventArgs. Last direct to each control.
//
// Args is of type "EventArgs". The first init argument is "sender", which is
// automatically bound to each receiving control. The second init argument is
// "original_sender", which is unchanged. And "args" will be perfectly forwarded
// as the rest arguments.
template <typename EventArgs, typename... Args>
void DispatchEvent(Control* const original_sender,
                   event::RoutedEvent<EventArgs>* (Control::*event_ptr)(),
                   Control* const last_receiver, Args&&... args) {
  std::list<Control*> receive_list;

  auto parent = original_sender;
  while (parent != last_receiver) {
    receive_list.push_back(parent);
    parent = parent->GetParent();
  }

  auto handled = false;

  // tunnel
  for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
    EventArgs event_args(*i, original_sender, std::forward<Args>(args)...);
    (*i->*event_ptr)()->tunnel.Raise(event_args);
    if (event_args.IsHandled()) {
      handled = true;
      break;
    }
  }

  // bubble
  if (!handled) {
    for (auto i : receive_list) {
      EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
      (i->*event_ptr)()->bubble.Raise(event_args);
      if (event_args.IsHandled()) break;
    }
  }

  // direct
  for (auto i : receive_list) {
    EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
    (i->*event_ptr)()->direct.Raise(event_args);
  }
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
    if (left_i == left_list.cend()) return *(--left_i);
    if (right_i == right_list.cend()) return *(--right_i);
    if (*left_i != *right_i) return *(--left_i);
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
void BindNativeEvent(Window* window, IEvent<T>* event,
                     void (Window::*handler)(typename IEvent<T>::EventArgs),
                     std::vector<EventRevokerGuard>& guard_pool) {
  guard_pool.push_back(EventRevokerGuard(
      event->AddHandler(std::bind(handler, window, std::placeholders::_1))));
}
}  // namespace

Window::Window(tag_overlapped_constructor)
    : mouse_hover_control_(nullptr), focus_control_(this) {
  native_window_ =
      platform::native::UiApplication::GetInstance()->CreateWindow(nullptr);
  render_object_.reset(new render::WindowRenderObject(this));

  BindNativeEvent(this, native_window_->DestroyEvent(),
                  &Window::OnNativeDestroy, event_revoker_guards_);
  BindNativeEvent(this, native_window_->PaintEvent(), &Window::OnNativePaint,
                  event_revoker_guards_);
  BindNativeEvent(this, native_window_->ResizeEvent(), &Window::OnNativeResize,
                  event_revoker_guards_);
  BindNativeEvent(this, native_window_->FocusEvent(), &Window::OnNativeFocus,
                  event_revoker_guards_);
  BindNativeEvent(this, native_window_->MouseEnterLeaveEvent(),
                  &Window::OnNativeMouseEnterLeave, event_revoker_guards_);
  BindNativeEvent(this, native_window_->MouseMoveEvent(),
                  &Window::OnNativeMouseMove, event_revoker_guards_);
  BindNativeEvent(this, native_window_->MouseDownEvent(),
                  &Window::OnNativeMouseDown, event_revoker_guards_);
  BindNativeEvent(this, native_window_->MouseUpEvent(),
                  &Window::OnNativeMouseUp, event_revoker_guards_);
  BindNativeEvent(this, native_window_->KeyDownEvent(),
                  &Window::OnNativeKeyDown, event_revoker_guards_);
  BindNativeEvent(this, native_window_->KeyUpEvent(), &Window::OnNativeKeyUp,
                  event_revoker_guards_);
}

Window::~Window() {
  TraverseDescendants(
      [this](Control* control) { control->OnDetachToWindow(this); });
}

std::wstring_view Window::GetControlType() const { return control_type; }

render::RenderObject* Window::GetRenderObject() const {
  return render_object_.get();
}

void Window::Relayout() { this->render_object_->MeasureAndLayout(); }

void Window::InvalidateLayout() {
  if (!need_layout_) {
    platform::native::UiApplication::GetInstance()->InvokeLater(
        [resolver = this->CreateResolver()] {
          if (const auto window = resolver.Resolve()) {
            window->Relayout();
            window->need_layout_ = false;
          }
      });
    need_layout_ = true;
  }
}

bool Window::RequestFocusFor(Control* control) {
  assert(control != nullptr);  // The control to request focus can't be null.
                               // You can set it as the window.

  if (focus_control_ == control) return true;

  DispatchEvent(focus_control_, &Control::LoseFocusEvent, nullptr, false);

  focus_control_ = control;

  DispatchEvent(control, &Control::GainFocusEvent, nullptr, false);

  return true;
}

Control* Window::GetFocusControl() { return focus_control_; }

void Window::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child) render_object_->RemoveChild(0);
  if (new_child) render_object_->AddChild(new_child->GetRenderObject(), 0);
}

Control* Window::HitTest(const Point& point) {
  return render_object_->HitTest(point)->GetAttachedControl();
}

void Window::OnNativeDestroy(std::nullptr_t) { delete this; }

void Window::OnNativePaint(std::nullptr_t) {
  const auto painter =
      std::unique_ptr<platform::graph::Painter>(native_window_->BeginPaint());
  render_object_->Draw(painter.get());
  painter->EndDraw();
}

void Window::OnNativeResize(const Size& size) {
  render_object_->MeasureAndLayout();
}

void Window::OnNativeFocus(bool focus) {
  focus
      ? DispatchEvent(focus_control_, &Control::GainFocusEvent, nullptr, true)
      : DispatchEvent(focus_control_, &Control::LoseFocusEvent, nullptr, true);
}

void Window::OnNativeMouseEnterLeave(bool enter) {
  if (!enter) {
    DispatchEvent(mouse_hover_control_, &Control::MouseLeaveEvent, nullptr);
    mouse_hover_control_ = nullptr;
  }
}

void Window::OnNativeMouseMove(const Point& point) {
  // Find the first control that hit test succeed.
  const auto new_control_mouse_hover = HitTest(point);
  const auto old_control_mouse_hover = mouse_hover_control_;
  mouse_hover_control_ = new_control_mouse_hover;

  DispatchMouseHoverControlChangeEvent(old_control_mouse_hover,
                                       new_control_mouse_hover, point);
  DispatchEvent(new_control_mouse_hover, &Control::MouseMoveEvent, nullptr,
                point);
}

void Window::OnNativeMouseDown(
    const platform::native::NativeMouseButtonEventArgs& args) {
  Control* control = HitTest(args.point);
  DispatchEvent(control, &Control::MouseDownEvent, nullptr, args.point,
                args.button);
}

void Window::OnNativeMouseUp(
    const platform::native::NativeMouseButtonEventArgs& args) {
  Control* control = HitTest(args.point);
  DispatchEvent(control, &Control::MouseUpEvent, nullptr, args.point,
                args.button);
}

void Window::OnNativeKeyDown(int virtual_code) {
  DispatchEvent(focus_control_, &Control::KeyDownEvent, nullptr, virtual_code);
}

void Window::OnNativeKeyUp(int virtual_code) {
  DispatchEvent(focus_control_, &Control::KeyUpEvent, nullptr, virtual_code);
}

void Window::DispatchMouseHoverControlChangeEvent(Control* old_control,
                                                  Control* new_control,
                                                  const Point& point) {
  if (new_control != old_control)  // if the mouse-hover-on control changed
  {
    const auto lowest_common_ancestor =
        FindLowestCommonAncestor(old_control, new_control);
    if (old_control != nullptr)  // if last mouse-hover-on control exists
      DispatchEvent(old_control, &Control::MouseLeaveEvent,
                    lowest_common_ancestor);  // dispatch mouse leave event.
    if (new_control != nullptr) {
      DispatchEvent(new_control, &Control::MouseEnterEvent,
                    lowest_common_ancestor,
                    point);  // dispatch mouse enter event.
    }
  }
}
}  // namespace cru::ui
