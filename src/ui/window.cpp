#include "cru/ui/window.hpp"

#include "cru/platform/graph/painter.hpp"
#include "cru/platform/native/native_window.hpp"
#include "cru/platform/native/ui_applicaition.hpp"
#include "cru/ui/render/window_render_object.hpp"
#include "routed_event_dispatch.hpp"

#include <cassert>

namespace cru::ui {
namespace {
std::list<Control*> GetAncestorList(Control* control) {
  std::list<Control*> l;
  while (control != nullptr) {
    l.push_front(control);
    control = control->GetParent();
  }
  return l;
}

constexpr int in_neither = 0;
constexpr int in_left = 1;
constexpr int in_right = 2;

// if find_control is not nullptr, then find_result will be set as where the
// find_control is located. in_neither means mouse hover state of it is not
// changed. in_left means mouse move out it. in_right means mouse move in it.
// This is useful for mouse capture.
Control* FindLowestCommonAncestor(Control* left, Control* right,
                                  Control* find_control, int* find_result) {
  if (find_control) {
    *find_result = in_neither;
  }

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
      Control* result = *(--left_i);
      while (right_i != right_list.cend()) {
        if (*right_i == find_control) {
          *find_result = in_right;
          return result;
        }
      }
      return result;
    }
    if (right_i == right_list.cend()) {
      Control* result = *(--right_i);
      while (left_i != left_list.cend()) {
        if (*left_i == find_control) {
          *find_result = in_left;
          return result;
        }
      }
      return result;
    }
    if (*left_i != *right_i) {
      Control* result = *(--left_i);
      ++left_i;
      while (right_i != right_list.cend()) {
        if (*right_i == find_control) {
          *find_result = in_right;
          return result;
        }
      }
      while (left_i != left_list.cend()) {
        if (*left_i == find_control) {
          *find_result = in_left;
          return result;
        }
      }
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
void BindNativeEvent(Window* window, IEvent<T>* event,
                     void (Window::*handler)(typename IEvent<T>::EventArgs),
                     std::vector<EventRevokerGuard>& guard_pool) {
  guard_pool.push_back(EventRevokerGuard(
      event->AddHandler(std::bind(handler, window, std::placeholders::_1))));
}
}  // namespace

Window::Window(tag_overlapped_constructor)
    : mouse_hover_control_(nullptr),
      focus_control_(this),
      mouse_captured_control_(nullptr) {
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

bool Window::CaptureMouseFor(Control* control) {
  if (control == nullptr) {
    if (mouse_captured_control_) {
      mouse_captured_control_ = nullptr;
      OnNativeMouseMove(GetNativeWindow()->GetMousePosition());
    }
    return true;
  }

  if (mouse_captured_control_) return false;
  mouse_captured_control_ = control;
  const auto c =
      FindLowestCommonAncestor(control, mouse_hover_control_, nullptr, nullptr);
  DispatchEvent(mouse_hover_control_, &Control::MouseLeaveEvent, c);
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
  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(control, &Control::MouseDownEvent, nullptr, args.point,
                args.button);
}

void Window::OnNativeMouseUp(
    const platform::native::NativeMouseButtonEventArgs& args) {
  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
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
    int find_result;
    const auto lowest_common_ancestor = FindLowestCommonAncestor(
        old_control, new_control, mouse_captured_control_, &find_result);
    if (old_control != nullptr &&  // if last mouse-hover-on control exists
        (mouse_captured_control_ == nullptr ||  // and if mouse is not captured
         find_result == in_left))  // or mouse is captured andc mouse is move
                                   // out of capturing control
      DispatchEvent(old_control, &Control::MouseLeaveEvent,
                    lowest_common_ancestor);  // dispatch mouse leave event.
    if (new_control != nullptr &&
        (mouse_captured_control_ == nullptr || find_result == in_right)) {
      DispatchEvent(new_control, &Control::MouseEnterEvent,
                    lowest_common_ancestor,
                    point);  // dispatch mouse enter event.
    }
  }
}
}  // namespace cru::ui
