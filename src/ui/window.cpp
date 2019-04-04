#include "cru/ui/window.hpp"

#include "cru/ui/render/window_render_object.hpp"
#include "cru/platform/ui_applicaition.hpp"
#include "cru/platform/native_window.hpp"

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


Window::Window(tag_overlapped_constructor) {
  native_window_ = platform::UiApplication::GetInstance()->CreateWindow(nullptr);
  render_object_.reset(new render::WindowRenderObject(this));
}

Window::~Window() {
  TraverseDescendants(
      [this](Control* control) { control->OnDetachToWindow(this); });
}

std::wstring_view Window::GetControlType() const { return control_type; }

render::RenderObject* Window::GetRenderObject() const {
  return render_object_.get();
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

void Window::OnNativeDestroy() { delete this; }

void Window::OnNativePaint() {
  render_target_->SetAsTarget();

  auto device_context =
      render_target_->GetGraphManager()->GetD2D1DeviceContext();
  device_context->BeginDraw();
  // Clear the background.
  device_context->Clear(D2D1::ColorF(D2D1::ColorF::White));
  render_object_->Draw(device_context);
  ThrowIfFailed(device_context->EndDraw(), "Failed to draw window.");
  render_target_->Present();

  ValidateRect(hwnd_, nullptr);
}

void Window::OnResizeInternal(const int new_width, const int new_height) {
  render_target_->ResizeBuffer(new_width, new_height);
  if (!(new_width == 0 && new_height == 0)) render_object_->MeasureAndLayout();
}

void Window::OnSetFocusInternal() {
  window_focus_ = true;
  DispatchEvent(focus_control_, &Control::GainFocusEvent, nullptr, true);
}

void Window::OnKillFocusInternal() {
  window_focus_ = false;
  DispatchEvent(focus_control_, &Control::LoseFocusEvent, nullptr, true);
}

void Window::OnMouseMoveInternal(const POINT point) {
  const auto dip_point = PiToDip(point);

  // when mouse was previous outside the window
  if (mouse_hover_control_ == nullptr) {
    // invoke TrackMouseEvent to have WM_MOUSELEAVE sent.
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof tme;
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hwnd_;

    TrackMouseEvent(&tme);
  }

  // Find the first control that hit test succeed.
  const auto new_control_mouse_hover = HitTest(dip_point);
  const auto old_control_mouse_hover = mouse_hover_control_;
  mouse_hover_control_ = new_control_mouse_hover;

  if (mouse_capture_control_)  // if mouse is captured
  {
    DispatchEvent(mouse_capture_control_, &Control::MouseMoveEvent, nullptr,
                  dip_point);
  } else {
    DispatchMouseHoverControlChangeEvent(old_control_mouse_hover,
                                         new_control_mouse_hover, dip_point);
    DispatchEvent(new_control_mouse_hover, &Control::MouseMoveEvent, nullptr,
                  dip_point);
  }
}

void Window::OnMouseLeaveInternal() {
  DispatchEvent(mouse_hover_control_, &Control::MouseLeaveEvent, nullptr);
  mouse_hover_control_ = nullptr;
}

void Window::OnMouseDownInternal(MouseButton button, POINT point) {
  const auto dip_point = PiToDip(point);

  Control* control;

  if (mouse_capture_control_)
    control = mouse_capture_control_;
  else
    control = HitTest(dip_point);

  DispatchEvent(control, &Control::MouseDownEvent, nullptr, dip_point,
                button);
}

void Window::OnMouseUpInternal(MouseButton button, POINT point) {
  const auto dip_point = PiToDip(point);

  Control* control;

  if (mouse_capture_control_)
    control = mouse_capture_control_;
  else
    control = HitTest(dip_point);

  DispatchEvent(control, &Control::MouseUpEvent, nullptr, dip_point, button);
}

void Window::OnMouseWheelInternal(short delta, POINT point) {
  const auto dip_point = PiToDip(point);

  Control* control;

  if (mouse_capture_control_)
    control = mouse_capture_control_;
  else
    control = HitTest(dip_point);

  DispatchEvent(control, &Control::MouseWheelEvent, nullptr, dip_point,
                static_cast<float>(delta));
}

void Window::OnKeyDownInternal(int virtual_code) {
  DispatchEvent(focus_control_, &Control::KeyDownEvent, nullptr,
                virtual_code);
}

void Window::OnKeyUpInternal(int virtual_code) {
  DispatchEvent(focus_control_, &Control::KeyUpEvent, nullptr, virtual_code);
}

void Window::OnCharInternal(wchar_t c) {
  DispatchEvent(focus_control_, &Control::CharEvent, nullptr, c);
}

void Window::OnActivatedInternal() {
  events::UiEventArgs args(this, this);
  activated_event_.Raise(args);
}

void Window::OnDeactivatedInternal() {
  events::UiEventArgs args(this, this);
  deactivated_event_.Raise(args);
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
