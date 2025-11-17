#include "cru/ui/controls/Window.h"

#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"

#include <cassert>

namespace cru::ui::controls {
Window::Window()
    : event_handling_count_(0),
      native_window_(CreateNativeWindow()),
      focus_control_(this),
      mouse_hover_control_(nullptr),
      mouse_captured_control_(nullptr),
      layout_prefer_to_fill_window_(true),
      attached_control_(nullptr) {
  GetContainerRenderObject()->SetDefaultHorizontalAlignment(Alignment::Stretch);
  GetContainerRenderObject()->SetDefaultVertialAlignment(Alignment::Stretch);
}

Window::~Window() {}

Window* Window::CreatePopup() {
  auto window = new Window();
  window->GetNativeWindow()->SetStyleFlag(
      platform::gui::WindowStyleFlags::NoCaptionAndBorder);
  window->SetGainFocusOnCreateAndDestroyWhenLoseFocus(true);
  return window;
}

std::string Window::GetControlType() const { return std::string(kControlType); }

void Window::SetAttachedControl(Control* control) {
  attached_control_ = control;
}

platform::gui::INativeWindow* Window::GetNativeWindow() {
  return native_window_.get();
}

void Window::SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value) {
  gain_focus_on_create_and_destroy_when_lose_focus_event_guard_.Clear();
  if (value) {
    gain_focus_on_create_and_destroy_when_lose_focus_event_guard_ +=
        native_window_->VisibilityChangeEvent()->AddHandler(
            [this](platform::gui::WindowVisibilityType type) {
              if (type == platform::gui::WindowVisibilityType::Show) {
                native_window_->RequestFocus();
              }
            });

    gain_focus_on_create_and_destroy_when_lose_focus_event_guard_ +=
        native_window_->FocusEvent()->AddHandler(
            [this](platform::gui::FocusChangeType type) {
              if (type == platform::gui::FocusChangeType::Lose) {
                native_window_->Close();
              }
            });
  }
}

namespace {
template <typename T>
inline void BindNativeEvent(
    Window* window, platform::gui::INativeWindow* native_window,
    IEvent<T>* event,
    void (Window::*handler)(platform::gui::INativeWindow*,
                            typename IEvent<T>::Args)) {
  event->AddHandler(
      std::bind(handler, window, native_window, std::placeholders::_1));
}
}  // namespace

namespace {
bool IsAncestor(Control* control, Control* ancestor) {
  while (control != nullptr) {
    if (control == ancestor) return true;
    control = control->GetParent();
  }
  return false;
}

// Ancestor at last.
std::vector<Control*> GetAncestorList(Control* control) {
  if (control == nullptr) return {};

  std::vector<Control*> l;
  while (control != nullptr) {
    l.push_back(control);
    control = control->GetParent();
  }
  return l;
}

Control* FindLowestCommonAncestor(Control* left, Control* right) {
  if (left == nullptr || right == nullptr) return nullptr;

  auto&& left_list = GetAncestorList(left);
  auto&& right_list = GetAncestorList(right);

  // the root is different
  if (left_list.back() != right_list.back()) return nullptr;

  // find the last same control or the last control (one is ancestor of the
  // other)
  auto left_iter = left_list.crbegin();
  auto right_iter = right_list.crbegin();

  while (true) {
    if (left_iter == left_list.crend()) {
      return left_list.front();
    }
    if (right_iter == right_list.crend()) {
      return right_list.front();
    }
    if (*left_iter != *right_iter) {
      return *(--left_iter);
    }
    ++left_iter;
    ++right_iter;
  }
}
}  // namespace

std::unique_ptr<platform::gui::INativeWindow> Window::CreateNativeWindow() {
  const auto ui_application = platform::gui::IUiApplication::GetInstance();

  auto native_window = ui_application->CreateWindow();
  assert(native_window);

  BindNativeEvent(this, native_window, native_window->DestroyEvent(),
                  &Window::OnNativeDestroy);
  BindNativeEvent(this, native_window, native_window->PaintEvent(),
                  &Window::OnNativePaint);
  BindNativeEvent(this, native_window, native_window->ResizeEvent(),
                  &Window::OnNativeResize);
  BindNativeEvent(this, native_window, native_window->FocusEvent(),
                  &Window::OnNativeFocus);
  BindNativeEvent(this, native_window, native_window->MouseEnterLeaveEvent(),
                  &Window::OnNativeMouseEnterLeave);
  BindNativeEvent(this, native_window, native_window->MouseMoveEvent(),
                  &Window::OnNativeMouseMove);
  BindNativeEvent(this, native_window, native_window->MouseDownEvent(),
                  &Window::OnNativeMouseDown);
  BindNativeEvent(this, native_window, native_window->MouseUpEvent(),
                  &Window::OnNativeMouseUp);
  BindNativeEvent(this, native_window, native_window->MouseWheelEvent(),
                  &Window::OnNativeMouseWheel);
  BindNativeEvent(this, native_window, native_window->KeyDownEvent(),
                  &Window::OnNativeKeyDown);
  BindNativeEvent(this, native_window, native_window->KeyUpEvent(),
                  &Window::OnNativeKeyUp);

  return std::unique_ptr<platform::gui::INativeWindow>(native_window);
}

void Window::InvalidatePaint() {
  repaint_schedule_canceler_.Reset(
      platform::gui::IUiApplication::GetInstance()->SetImmediate(
          [this] { Repaint(); }));
}

void Window::InvalidateLayout() {
  relayout_schedule_canceler_.Reset(
      platform::gui::IUiApplication::GetInstance()->SetImmediate(
          [this] { Relayout(); }));
}

bool Window::IsLayoutPreferToFillWindow() const {
  return layout_prefer_to_fill_window_;
}

void Window::SetLayoutPreferToFillWindow(bool value) {
  if (value == layout_prefer_to_fill_window_) return;
  layout_prefer_to_fill_window_ = value;
  InvalidateLayout();
}

void Window::Repaint() {
  auto painter = native_window_->BeginPaint();
  painter->Clear(colors::white);
  GetRenderObject()->Draw(painter.get());
  painter->EndDraw();
}

void Window::Relayout() { RelayoutWithSize(native_window_->GetClientSize()); }

void Window::RelayoutWithSize(const Size& available_size,
                              bool set_window_size_to_fit_content) {
  auto render_object = GetRenderObject();
  render_object->Measure(
      render::MeasureRequirement{
          available_size,
          !set_window_size_to_fit_content && IsLayoutPreferToFillWindow()
              ? render::MeasureSize(available_size)
              : render::MeasureSize::NotSpecified()},
      render::MeasureSize::NotSpecified());

  if (set_window_size_to_fit_content) {
    native_window_->SetClientSize(render_object->GetDesiredSize());
  }

  render_object->Layout(Point{});
  CRU_LOG_TAG_DEBUG("A relayout is finished.");

  AfterLayoutEvent_.Raise(nullptr);

  InvalidatePaint();
}

Control* Window::GetFocusControl() { return focus_control_; }

void Window::SetFocusControl(Control* control) {
  if (control == nullptr) control = this;
  if (focus_control_ == control) return;

  const auto old_focus_control = focus_control_;

  focus_control_ = control;

  DispatchFocusControlChangeEvent(old_focus_control, focus_control_, false);
}

Control* Window::GetMouseCaptureControl() { return mouse_captured_control_; }

bool Window::SetMouseCaptureControl(Control* control) {
  if (!native_window_->CaptureMouse()) return false;

  if (control == mouse_captured_control_) return true;

  if (control == nullptr) {
    native_window_->ReleaseMouse();
    const auto old_capture_control = mouse_captured_control_;
    mouse_captured_control_ =
        nullptr;  // update this in case this is used in event handlers
    if (old_capture_control != mouse_hover_control_) {
      DispatchMouseHoverControlChangeEvent(
          old_capture_control, mouse_hover_control_,
          native_window_->GetMousePosition(), true, false);
    }
    UpdateCursor();
    return true;
  }

  if (mouse_captured_control_) return false;

  mouse_captured_control_ = control;
  DispatchMouseHoverControlChangeEvent(
      mouse_hover_control_, mouse_captured_control_,
      native_window_->GetMousePosition(), false, true);
  UpdateCursor();
  return true;
}

std::shared_ptr<platform::gui::ICursor> Window::GetOverrideCursor() {
  return override_cursor_;
}

void Window::SetOverrideCursor(std::shared_ptr<platform::gui::ICursor> cursor) {
  if (cursor == override_cursor_) return;
  override_cursor_ = cursor;
  UpdateCursor();
}

bool Window::IsInEventHandling() { return event_handling_count_; }

void Window::OnNativeDestroy(platform::gui::INativeWindow* window,
                             std::nullptr_t) {
  CRU_UNUSED(window)
}

void Window::OnNativePaint(platform::gui::INativeWindow* window,
                           std::nullptr_t) {
  CRU_UNUSED(window)
  InvalidatePaint();
}

void Window::OnNativeResize(platform::gui::INativeWindow* window,
                            const Size& size) {
  CRU_UNUSED(window)
  CRU_UNUSED(size)

  InvalidateLayout();
}

void Window::OnNativeFocus(platform::gui::INativeWindow* window,
                           platform::gui::FocusChangeType focus) {
  CRU_UNUSED(window)

  focus == platform::gui::FocusChangeType::Gain
      ? DispatchEvent(focus_control_, &Control::GainFocusEvent, nullptr, true)
      : DispatchEvent(focus_control_, &Control::LoseFocusEvent, nullptr, true);
}

void Window::OnNativeMouseEnterLeave(platform::gui::INativeWindow* window,
                                     platform::gui::MouseEnterLeaveType type) {
  CRU_UNUSED(window)

  if (type == platform::gui::MouseEnterLeaveType::Leave) {
    DispatchEvent(mouse_hover_control_, &Control::MouseLeaveEvent, nullptr);
    mouse_hover_control_ = nullptr;
  }
}

void Window::OnNativeMouseMove(platform::gui::INativeWindow* window,
                               const Point& point) {
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
      DispatchEvent(o, &Control::MouseLeaveEvent, n);
    } else {
      DispatchEvent(n, &Control::MouseEnterEvent, o, point);
    }
    DispatchEvent(mouse_captured_control_, &Control::MouseMoveEvent, nullptr,
                  point);
    UpdateCursor();
    return;
  }

  DispatchMouseHoverControlChangeEvent(
      old_mouse_hover_control, new_mouse_hover_control, point, false, false);
  DispatchEvent(new_mouse_hover_control, &Control::MouseMoveEvent, nullptr,
                point);
  UpdateCursor();
}

void Window::OnNativeMouseDown(
    platform::gui::INativeWindow* window,
    const platform::gui::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(control, &Control::MouseDownEvent, nullptr, args.point,
                args.button, args.modifier);
}

void Window::OnNativeMouseUp(
    platform::gui::INativeWindow* window,
    const platform::gui::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(control, &Control::MouseUpEvent, nullptr, args.point,
                args.button, args.modifier);
}

void Window::OnNativeMouseWheel(
    platform::gui::INativeWindow* window,
    const platform::gui::NativeMouseWheelEventArgs& args) {
  CRU_UNUSED(window)

  Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(control, &Control::MouseWheelEvent, nullptr, args.point,
                args.delta, args.modifier);
}

void Window::OnNativeKeyDown(platform::gui::INativeWindow* window,
                             const platform::gui::NativeKeyEventArgs& args) {
  CRU_UNUSED(window)

  DispatchEvent(focus_control_, &Control::KeyDownEvent, nullptr, args.key,
                args.modifier);
}

void Window::OnNativeKeyUp(platform::gui::INativeWindow* window,
                           const platform::gui::NativeKeyEventArgs& args) {
  CRU_UNUSED(window)

  DispatchEvent(focus_control_, &Control::KeyUpEvent, nullptr, args.key,
                args.modifier);
}

void Window::DispatchFocusControlChangeEvent(Control* old_control,
                                             Control* new_control,
                                             bool is_window) {
  if (new_control != old_control) {
    const auto lowest_common_ancestor =
        FindLowestCommonAncestor(old_control, new_control);
    DispatchEvent(old_control, &Control::LoseFocusEvent, lowest_common_ancestor,
                  is_window);
    DispatchEvent(new_control, &Control::GainFocusEvent, lowest_common_ancestor,
                  is_window);
  }
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
      DispatchEvent(old_control, &Control::MouseLeaveEvent,
                    lowest_common_ancestor);  // dispatch mouse leave event.
    if (!no_enter && new_control != nullptr) {
      DispatchEvent(new_control, &Control::MouseEnterEvent,
                    lowest_common_ancestor,
                    point);  // dispatch mouse enter event.
    }
  }
}

void Window::UpdateCursor() {
  if (override_cursor_) {
    native_window_->SetCursor(override_cursor_);
  } else {
    const auto capture = GetMouseCaptureControl();
    native_window_->SetCursor(
        (capture ? capture : GetMouseHoverControl())->GetInheritedCursor());
  }
}

void Window::NotifyControlDestroyed(Control* control) {
  if (focus_control_->HasAncestor(control)) {
    focus_control_ = control->GetParent();
  }

  if (mouse_captured_control_->HasAncestor(control)) {
    mouse_captured_control_ = control->GetParent();
  }

  if (mouse_hover_control_->HasAncestor(control)) {
    mouse_hover_control_ = control->GetParent();
  }
}
}  // namespace cru::ui::controls
