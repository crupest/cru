#include "cru/ui/host/WindowHost.h"

#include "RoutedEventDispatch.h"
#include "cru/common/Base.h"
#include "cru/common/Logger.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/InputMethod.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/host/LayoutPaintCycler.h"
#include "cru/ui/render/MeasureRequirement.h"
#include "cru/ui/render/RenderObject.h"

#include <cstddef>
#include <memory>

namespace cru::ui::host {
using platform::gui::INativeWindow;
using platform::gui::IUiApplication;

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
CRU_DEFINE_EVENT_NAME(MouseWheel)
CRU_DEFINE_EVENT_NAME(KeyDown)
CRU_DEFINE_EVENT_NAME(KeyUp)

#undef CRU_DEFINE_EVENT_NAME
}  // namespace event_names

namespace {
bool IsAncestor(controls::Control* control, controls::Control* ancestor) {
  while (control != nullptr) {
    if (control == ancestor) return true;
    control = control->GetParent();
  }
  return false;
}

// Ancestor at last.
std::vector<controls::Control*> GetAncestorList(controls::Control* control) {
  std::vector<controls::Control*> l;
  while (control != nullptr) {
    l.push_back(control);
    control = control->GetParent();
  }
  return l;
}

controls::Control* FindLowestCommonAncestor(controls::Control* left,
                                            controls::Control* right) {
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

namespace {
template <typename T>
inline void BindNativeEvent(
    WindowHost* host, INativeWindow* native_window, IEvent<T>* event,
    void (WindowHost::*handler)(INativeWindow*, typename IEvent<T>::EventArgs),
    std::vector<EventRevokerGuard>& guard_pool) {
  guard_pool.push_back(EventRevokerGuard(event->AddHandler(
      std::bind(handler, host, native_window, std::placeholders::_1))));
}
}  // namespace

WindowHost::WindowHost(controls::Control* root_control)
    : root_control_(root_control), focus_control_(root_control) {
  root_control_->TraverseDescendants([this](controls::Control* control) {
    control->window_host_ = this;
    control->OnAttachToHost(this);
  });

  root_render_object_ = root_control->GetRenderObject();
  root_render_object_->SetWindowHostRecursive(this);

  this->layout_paint_cycler_ = std::make_unique<LayoutPaintCycler>(this);

  CreateNativeWindow();
}

WindowHost::~WindowHost() { delete native_window_; }

gsl::not_null<platform::gui::INativeWindow*> WindowHost::CreateNativeWindow() {
  const auto ui_application = IUiApplication::GetInstance();

  auto native_window = ui_application->CreateWindow();
  Ensures(native_window);

  native_window_ = native_window;

  BindNativeEvent(this, native_window, native_window->DestroyEvent(),
                  &WindowHost::OnNativeDestroy, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->PaintEvent(),
                  &WindowHost::OnNativePaint, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->ResizeEvent(),
                  &WindowHost::OnNativeResize, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->FocusEvent(),
                  &WindowHost::OnNativeFocus, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseEnterLeaveEvent(),
                  &WindowHost::OnNativeMouseEnterLeave, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseMoveEvent(),
                  &WindowHost::OnNativeMouseMove, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseDownEvent(),
                  &WindowHost::OnNativeMouseDown, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseUpEvent(),
                  &WindowHost::OnNativeMouseUp, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->MouseWheelEvent(),
                  &WindowHost::OnNativeMouseWheel, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->KeyDownEvent(),
                  &WindowHost::OnNativeKeyDown, event_revoker_guards_);
  BindNativeEvent(this, native_window, native_window->KeyUpEvent(),
                  &WindowHost::OnNativeKeyUp, event_revoker_guards_);

  native_window_change_event_.Raise(native_window);

  return native_window_;
}

void WindowHost::InvalidatePaint() { layout_paint_cycler_->InvalidatePaint(); }

void WindowHost::InvalidateLayout() {
  layout_paint_cycler_->InvalidateLayout();
}

bool WindowHost::IsLayoutPreferToFillWindow() const {
  return layout_prefer_to_fill_window_;
}

void WindowHost::SetLayoutPreferToFillWindow(bool value) {
  if (value == layout_prefer_to_fill_window_) return;
  layout_prefer_to_fill_window_ = value;
  InvalidateLayout();
}

void WindowHost::Relayout() {
  const auto available_size =
      native_window_ ? native_window_->GetClientSize() : Size::Infinate();
  RelayoutWithSize(available_size);
}

void WindowHost::RelayoutWithSize(const Size& available_size,
                                  bool set_window_size_to_fit_content) {
  root_render_object_->Measure(
      render::MeasureRequirement{
          available_size,
          !set_window_size_to_fit_content && IsLayoutPreferToFillWindow()
              ? render::MeasureSize(available_size)
              : render::MeasureSize::NotSpecified()},
      render::MeasureSize::NotSpecified());

  if (set_window_size_to_fit_content) {
    native_window_->SetClientSize(root_render_object_->GetSize());
  }

  root_render_object_->Layout(Point{});
  for (auto& action : after_layout_stable_action_) action();
  after_layout_event_.Raise(AfterLayoutEventArgs{});
  root_render_object_->TraverseDescendants(
      [](render::RenderObject* render_object) {
        render_object->OnAfterLayout();
      });
  after_layout_stable_action_.clear();
  if constexpr (debug_flags::layout)
    log::TagDebug(log_tag, u"A relayout is finished.");
}

void WindowHost::Repaint() {
  if (native_window_ == nullptr) return;
  auto painter = native_window_->BeginPaint();
  painter->Clear(colors::white);
  root_render_object_->Draw(painter.get());
  painter->EndDraw();
}

controls::Control* WindowHost::GetFocusControl() { return focus_control_; }

void WindowHost::SetFocusControl(controls::Control* control) {
  if (focus_control_ == control) return;
  if (control == nullptr) control = root_control_;

  const auto old_focus_control = focus_control_;

  focus_control_ = control;

  DispatchEvent(event_names::LoseFocus, old_focus_control,
                &controls::Control::LoseFocusEvent, nullptr, false);

  DispatchEvent(event_names::GainFocus, control,
                &controls::Control::GainFocusEvent, nullptr, false);
}

bool WindowHost::CaptureMouseFor(controls::Control* control) {
  if (!native_window_) return false;
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

controls::Control* WindowHost::GetMouseCaptureControl() {
  return mouse_captured_control_;
}

void WindowHost::RunAfterLayoutStable(std::function<void()> action) {
  if (layout_paint_cycler_->IsLayoutDirty()) {
    after_layout_stable_action_.push_back(std::move(action));
  } else {
    action();
  }
}

void WindowHost::OnNativeDestroy(INativeWindow* window, std::nullptr_t) {
  CRU_UNUSED(window)
}

void WindowHost::OnNativePaint(INativeWindow* window, std::nullptr_t) {
  CRU_UNUSED(window)
  layout_paint_cycler_->InvalidatePaint();
}

void WindowHost::OnNativeResize(INativeWindow* window, const Size& size) {
  CRU_UNUSED(window)
  CRU_UNUSED(size)

  InvalidateLayout();
}

void WindowHost::OnNativeFocus(INativeWindow* window,
                               platform::gui::FocusChangeType focus) {
  CRU_UNUSED(window)

  focus == platform::gui::FocusChangeType::Gain
      ? DispatchEvent(event_names::GainFocus, focus_control_,
                      &controls::Control::GainFocusEvent, nullptr, true)
      : DispatchEvent(event_names::LoseFocus, focus_control_,
                      &controls::Control::LoseFocusEvent, nullptr, true);
}

void WindowHost::OnNativeMouseEnterLeave(
    INativeWindow* window, platform::gui::MouseEnterLeaveType type) {
  CRU_UNUSED(window)

  if (type == platform::gui::MouseEnterLeaveType::Leave) {
    DispatchEvent(event_names::MouseLeave, mouse_hover_control_,
                  &controls::Control::MouseLeaveEvent, nullptr);
    mouse_hover_control_ = nullptr;
  }
}

void WindowHost::OnNativeMouseMove(INativeWindow* window, const Point& point) {
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
      DispatchEvent(event_names::MouseLeave, o,
                    &controls::Control::MouseLeaveEvent, n);
    } else {
      DispatchEvent(event_names::MouseEnter, n,
                    &controls::Control::MouseEnterEvent, o, point);
    }
    DispatchEvent(event_names::MouseMove, mouse_captured_control_,
                  &controls::Control::MouseMoveEvent, nullptr, point);
    UpdateCursor();
    return;
  }

  DispatchMouseHoverControlChangeEvent(
      old_mouse_hover_control, new_mouse_hover_control, point, false, false);
  DispatchEvent(event_names::MouseMove, new_mouse_hover_control,
                &controls::Control::MouseMoveEvent, nullptr, point);
  UpdateCursor();
}

void WindowHost::OnNativeMouseDown(
    INativeWindow* window,
    const platform::gui::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  controls::Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(event_names::MouseDown, control,
                &controls::Control::MouseDownEvent, nullptr, args.point,
                args.button, args.modifier);
}

void WindowHost::OnNativeMouseUp(
    INativeWindow* window,
    const platform::gui::NativeMouseButtonEventArgs& args) {
  CRU_UNUSED(window)

  controls::Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(event_names::MouseUp, control, &controls::Control::MouseUpEvent,
                nullptr, args.point, args.button, args.modifier);
}

void WindowHost::OnNativeMouseWheel(
    platform::gui::INativeWindow* window,
    const platform::gui::NativeMouseWheelEventArgs& args) {
  CRU_UNUSED(window)

  controls::Control* control =
      mouse_captured_control_ ? mouse_captured_control_ : HitTest(args.point);
  DispatchEvent(event_names::MouseWheel, control,
                &controls::Control::MouseWheelEvent, nullptr, args.point,
                args.delta, args.modifier);
}

void WindowHost::OnNativeKeyDown(
    INativeWindow* window, const platform::gui::NativeKeyEventArgs& args) {
  CRU_UNUSED(window)

  DispatchEvent(event_names::KeyDown, focus_control_,
                &controls::Control::KeyDownEvent, nullptr, args.key,
                args.modifier);
}

void WindowHost::OnNativeKeyUp(INativeWindow* window,
                               const platform::gui::NativeKeyEventArgs& args) {
  CRU_UNUSED(window)

  DispatchEvent(event_names::KeyUp, focus_control_,
                &controls::Control::KeyUpEvent, nullptr, args.key,
                args.modifier);
}

void WindowHost::DispatchMouseHoverControlChangeEvent(
    controls::Control* old_control, controls::Control* new_control,
    const Point& point, bool no_leave, bool no_enter) {
  if (new_control != old_control)  // if the mouse-hover-on control changed
  {
    const auto lowest_common_ancestor =
        FindLowestCommonAncestor(old_control, new_control);
    if (!no_leave && old_control != nullptr)
      DispatchEvent(event_names::MouseLeave, old_control,
                    &controls::Control::MouseLeaveEvent,
                    lowest_common_ancestor);  // dispatch mouse leave event.
    if (!no_enter && new_control != nullptr) {
      DispatchEvent(event_names::MouseEnter, new_control,
                    &controls::Control::MouseEnterEvent, lowest_common_ancestor,
                    point);  // dispatch mouse enter event.
    }
  }
}

void WindowHost::UpdateCursor() {
  if (native_window_) {
    if (override_cursor_) {
      native_window_->SetCursor(override_cursor_);
    } else {
      const auto capture = GetMouseCaptureControl();
      native_window_->SetCursor(
          (capture ? capture : GetMouseHoverControl())->GetInheritedCursor());
    }
  }
}

controls::Control* WindowHost::HitTest(const Point& point) {
  const auto render_object = root_render_object_->HitTest(point);
  if (render_object) {
    const auto control = render_object->GetAttachedControl();
    Ensures(control);
    return control;
  }
  return root_control_;
}

std::shared_ptr<platform::gui::ICursor> WindowHost::GetOverrideCursor() {
  return override_cursor_;
}

void WindowHost::SetOverrideCursor(
    std::shared_ptr<platform::gui::ICursor> cursor) {
  if (cursor == override_cursor_) return;
  override_cursor_ = cursor;
  UpdateCursor();
}
}  // namespace cru::ui::host
