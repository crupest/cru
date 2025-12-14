#include "cru/ui/controls/ControlHost.h"

#include "cru/base/log/Logger.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"
#include "cru/ui/render/RenderObject.h"

#include <cassert>

namespace cru::ui::controls {
ControlHost::ControlHost(Control* root_control)
    : event_handling_count_(0),
      root_control_(root_control),
      native_window_(CreateNativeWindow()),
      focus_control_(root_control),
      mouse_hover_control_(nullptr),
      mouse_captured_control_(nullptr),
      layout_prefer_to_fill_window_(true) {
  root_control_->TraverseDescendents(
      [this](Control* control) { control->host_ = this; }, true);
}

ControlHost::~ControlHost() {
  root_control_->TraverseDescendents(
      [this](Control* control) { control->host_ = nullptr; }, true);
  root_control_->TraverseDescendents(
      [this](Control* control) {
        control->ControlHostChangeEvent_.Raise({this, nullptr});
      },
      true);
}

platform::gui::INativeWindow* ControlHost::GetNativeWindow() {
  return native_window_.get();
}

namespace {
template <typename T>
inline void BindNativeEvent(
    ControlHost* host, platform::gui::INativeWindow* native_window,
    IEvent<T>* event, void (ControlHost::*handler)(typename IEvent<T>::Args)) {
  event->AddHandler(std::bind(handler, host, std::placeholders::_1));
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

std::unique_ptr<platform::gui::INativeWindow>
ControlHost::CreateNativeWindow() {
  const auto ui_application = platform::gui::IUiApplication::GetInstance();

  auto native_window = ui_application->CreateWindow();
  assert(native_window);

  BindNativeEvent(this, native_window, native_window->DestroyEvent(),
                  &ControlHost::OnNativeDestroy);
  BindNativeEvent(this, native_window, native_window->Paint1Event(),
                  &ControlHost::OnNativePaint1);
  BindNativeEvent(this, native_window, native_window->ResizeEvent(),
                  &ControlHost::OnNativeResize);
  BindNativeEvent(this, native_window, native_window->FocusEvent(),
                  &ControlHost::OnNativeFocus);
  BindNativeEvent(this, native_window, native_window->MouseEnterLeaveEvent(),
                  &ControlHost::OnNativeMouseEnterLeave);
  BindNativeEvent(this, native_window, native_window->MouseMoveEvent(),
                  &ControlHost::OnNativeMouseMove);
  BindNativeEvent(this, native_window, native_window->MouseDownEvent(),
                  &ControlHost::OnNativeMouseDown);
  BindNativeEvent(this, native_window, native_window->MouseUpEvent(),
                  &ControlHost::OnNativeMouseUp);
  BindNativeEvent(this, native_window, native_window->MouseWheelEvent(),
                  &ControlHost::OnNativeMouseWheel);
  BindNativeEvent(this, native_window, native_window->KeyDownEvent(),
                  &ControlHost::OnNativeKeyDown);
  BindNativeEvent(this, native_window, native_window->KeyUpEvent(),
                  &ControlHost::OnNativeKeyUp);

  return std::unique_ptr<platform::gui::INativeWindow>(native_window);
}

void ControlHost::ScheduleRepaint() { native_window_->RequestRepaint(); }

void ControlHost::ScheduleRelayout() {
  relayout_schedule_canceler_.Reset(
      platform::gui::IUiApplication::GetInstance()->SetImmediate(
          [this] { Relayout(); }));
}

Rect ControlHost::GetPaintInvalidArea() { return paint_invalid_area_; }

void ControlHost::AddPaintInvalidArea(const Rect& area) {
  paint_invalid_area_ = paint_invalid_area_.Union(area);
}

void ControlHost::Repaint() {
  auto painter = native_window_->BeginPaint();
  painter->Clear(colors::white);
  render::RenderObjectDrawContext context{paint_invalid_area_, painter.get()};
  root_control_->GetRenderObject()->Draw(context);
  painter->EndDraw();
  paint_invalid_area_ = {};
}

void ControlHost::Relayout() {
  auto size = native_window_->GetClientSize();
  if (size.width == 0.f && size.height == 0.f) {
    size = Size::Infinite();
  }
  RelayoutWithSize(size);
}

void ControlHost::RelayoutWithSize(const Size& available_size,
                                   bool set_window_size_to_fit_content) {
  auto render_object = root_control_->GetRenderObject();
  render_object->Measure(render::MeasureRequirement{
      available_size,
      !set_window_size_to_fit_content && IsLayoutPreferToFillWindow()
          ? render::MeasureSize(available_size)
          : render::MeasureSize::NotSpecified(),
      render::MeasureSize::NotSpecified()});

  if (set_window_size_to_fit_content) {
    native_window_->SetClientSize(render_object->GetMeasureResultSize());
  }

  render_object->Layout(Point{});
  CruLogDebug(kLogTag, "A relayout is finished.");

  AfterLayoutEvent_.Raise(nullptr);

  ScheduleRepaint();
}

bool ControlHost::IsLayoutPreferToFillWindow() const {
  return layout_prefer_to_fill_window_;
}

void ControlHost::SetLayoutPreferToFillWindow(bool value) {
  if (value == layout_prefer_to_fill_window_) return;
  layout_prefer_to_fill_window_ = value;
  ScheduleRelayout();
}

Control* ControlHost::GetFocusControl() { return focus_control_; }

void ControlHost::SetFocusControl(Control* control) {
  if (control == nullptr) control = root_control_;
  if (focus_control_ == control) return;

  const auto old_focus_control = focus_control_;

  focus_control_ = control;

  DispatchFocusControlChangeEvent(old_focus_control, focus_control_, false);
}

Control* ControlHost::GetMouseCaptureControl() {
  return mouse_captured_control_;
}

bool ControlHost::SetMouseCaptureControl(Control* control) {
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

std::shared_ptr<platform::gui::ICursor> ControlHost::GetOverrideCursor() {
  return override_cursor_;
}

void ControlHost::SetOverrideCursor(
    std::shared_ptr<platform::gui::ICursor> cursor) {
  if (cursor == override_cursor_) return;
  override_cursor_ = cursor;
  UpdateCursor();
}

bool ControlHost::IsInEventHandling() { return event_handling_count_; }

void ControlHost::OnNativeDestroy(std::nullptr_t) {
  auto old_hover = mouse_hover_control_;
  mouse_hover_control_ = nullptr;
  DispatchMouseHoverControlChangeEvent(old_hover, nullptr, {}, false, false);
  mouse_captured_control_ = nullptr;
}

void ControlHost::OnNativePaint1(
    const platform::gui::NativePaintEventArgs& args) {
  AddPaintInvalidArea(args.repaint_area);
  Repaint();
}

void ControlHost::OnNativeResize([[maybe_unused]] const Size& size) {
  CruLogDebug(kLogTag, "Window resize to {}.", size);
  ScheduleRelayout();
}

void ControlHost::OnNativeFocus(platform::gui::FocusChangeType focus) {
  focus == platform::gui::FocusChangeType::Gain
      ? DispatchEvent(focus_control_, &Control::GainFocusEvent, nullptr, true)
      : DispatchEvent(focus_control_, &Control::LoseFocusEvent, nullptr, true);
}

void ControlHost::OnNativeMouseEnterLeave(
    platform::gui::MouseEnterLeaveType type) {
  if (type == platform::gui::MouseEnterLeaveType::Leave) {
    DispatchEvent(mouse_hover_control_, &Control::MouseLeaveEvent, nullptr);
    mouse_hover_control_ = nullptr;
  }
}

void ControlHost::OnNativeMouseMove(const Point& point) {
  // Find the first control that hit test succeed.
  const auto new_mouse_hover_control = root_control_->HitTest(point);
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

void ControlHost::OnNativeMouseDown(
    const platform::gui::NativeMouseButtonEventArgs& args) {
  Control* control = mouse_captured_control_
                         ? mouse_captured_control_
                         : root_control_->HitTest(args.point);
  DispatchEvent(control, &Control::MouseDownEvent, nullptr, args.point,
                args.button, args.modifier);
}

void ControlHost::OnNativeMouseUp(
    const platform::gui::NativeMouseButtonEventArgs& args) {
  Control* control = mouse_captured_control_
                         ? mouse_captured_control_
                         : root_control_->HitTest(args.point);
  DispatchEvent(control, &Control::MouseUpEvent, nullptr, args.point,
                args.button, args.modifier);
}

void ControlHost::OnNativeMouseWheel(
    const platform::gui::NativeMouseWheelEventArgs& args) {
  Control* control = mouse_captured_control_
                         ? mouse_captured_control_
                         : root_control_->HitTest(args.point);
  DispatchEvent(control, &Control::MouseWheelEvent, nullptr, args.point,
                args.delta, args.modifier);
}

void ControlHost::OnNativeKeyDown(
    const platform::gui::NativeKeyEventArgs& args) {
  DispatchEvent(focus_control_, &Control::KeyDownEvent, nullptr, args.key,
                args.modifier);
}

void ControlHost::OnNativeKeyUp(const platform::gui::NativeKeyEventArgs& args) {
  DispatchEvent(focus_control_, &Control::KeyUpEvent, nullptr, args.key,
                args.modifier);
}

void ControlHost::DispatchFocusControlChangeEvent(Control* old_control,
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

void ControlHost::DispatchMouseHoverControlChangeEvent(Control* old_control,
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

void ControlHost::UpdateCursor() {
  if (override_cursor_) {
    native_window_->SetCursor(override_cursor_);
  } else {
    const auto capture = GetMouseCaptureControl();
    native_window_->SetCursor(
        (capture ? capture : GetMouseHoverControl())->GetInheritedCursor());
  }
}

void ControlHost::NotifyControlParentChange(Control* control,
                                            Control* old_parent,
                                            Control* new_parent) {
  if (new_parent == nullptr) {
    if (focus_control_->HasAncestor(control)) {
      focus_control_ = old_parent;
    }

    if (mouse_captured_control_->HasAncestor(control)) {
      mouse_captured_control_ = old_parent;
    }

    if (mouse_hover_control_->HasAncestor(control)) {
      mouse_hover_control_ = old_parent;
    }
  }
}
}  // namespace cru::ui::controls
