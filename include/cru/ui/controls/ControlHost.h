#pragma once
#include "Control.h"

#include <cru/base/Base.h>
#include <cru/base/Event.h>
#include <cru/base/Guard.h>
#include <cru/base/log/Logger.h>
#include <cru/platform/gui/UiApplication.h>
#include <cru/platform/gui/Window.h>

namespace cru::ui::controls {
class CRU_UI_API ControlHost : public Object {
 private:
  constexpr static auto kLogTag = "cru::ui::controls::ControlHost";
  friend Control;

 public:
  explicit ControlHost(Control* root_control);
  ~ControlHost() override;

  platform::gui::INativeWindow* GetNativeWindow();

  void ScheduleRepaint();
  void ScheduleRelayout();

  Rect GetPaintInvalidArea();
  void AddPaintInvalidArea(const Rect& area);

  void Repaint();
  void Relayout();
  void RelayoutWithSize(const Size& available_size = Size::Infinite(),
                        bool set_window_size_to_fit_content = false);

  // If true, preferred size of root render object is set to window size when
  // measure. Default is true.
  bool IsLayoutPreferToFillWindow() const;
  void SetLayoutPreferToFillWindow(bool value);

  // Get current control that mouse hovers on. This ignores the mouse-capture
  // control. Even when mouse is captured by another control, this function
  // return the control under cursor. You can use `GetMouseCaptureControl` to
  // get more info.
  Control* GetMouseHoverControl() const { return mouse_hover_control_; }

  Control* GetFocusControl();
  void SetFocusControl(Control* control);

  Control* GetMouseCaptureControl();
  bool SetMouseCaptureControl(Control* control);

  std::shared_ptr<platform::gui::ICursor> GetOverrideCursor();
  void SetOverrideCursor(std::shared_ptr<platform::gui::ICursor> cursor);

  bool IsInEventHandling();

  CRU_DEFINE_EVENT(AfterLayout, std::nullptr_t)

 private:
  std::unique_ptr<platform::gui::INativeWindow> CreateNativeWindow();

  void OnNativeDestroy(std::nullptr_t);
  void OnNativePaint1(const cru::platform::gui::NativePaintEventArgs& args);
  void OnNativeResize(const Size& size);
  void OnNativeFocus(cru::platform::gui::FocusChangeType focus);
  void OnNativeMouseEnterLeave(cru::platform::gui::MouseEnterLeaveType enter);
  void OnNativeMouseMove(const Point& point);
  void OnNativeMouseDown(const platform::gui::NativeMouseButtonEventArgs& args);
  void OnNativeMouseUp(const platform::gui::NativeMouseButtonEventArgs& args);
  void OnNativeMouseWheel(const platform::gui::NativeMouseWheelEventArgs& args);
  void OnNativeKeyDown(const platform::gui::NativeKeyEventArgs& args);
  void OnNativeKeyUp(const platform::gui::NativeKeyEventArgs& args);

  void DispatchFocusControlChangeEvent(Control* old_control,
                                       Control* new_control, bool is_window);
  void DispatchMouseHoverControlChangeEvent(Control* old_control,
                                            Control* new_control,
                                            const Point& point, bool no_leave,
                                            bool no_enter);

  template <typename EventArgs, typename... Args>
  void DispatchEvent(Control* const original_sender,
                     events::RoutedEvent<EventArgs>* (Control::*event_ptr)(),
                     Control* const last_receiver, Args&&... args) {
    constexpr auto kLogTag = "cru::ui::controls::DispatchEvent";

    event_handling_count_++;
    Guard event_handling_count_guard([this] { event_handling_count_--; });

    if (original_sender == nullptr || original_sender == last_receiver) return;

    std::string log = "Begin dispatching routed event " +
                      (original_sender->*event_ptr)()->GetName() +
                      ":\n\tTunnel:";

    Guard logging_guard([&] {
      log += "\nEnd dispatching routed event " +
             (original_sender->*event_ptr)()->GetName() + ".";
      CruLogDebug(kLogTag, "{}", log);
    });

    std::vector<ObjectResolver<Control>> receive_list;

    auto parent = original_sender;
    while (parent != last_receiver) {
      receive_list.push_back(parent->CreateResolver());
      parent = parent->GetParent();
    }

    auto handled = false;

    // tunnel
    for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
      auto control = i->Resolve();
      log += " ";
      if (!control) {
        log += "(deleted)";
        continue;
      }
      log += control->GetDebugId();
      EventArgs event_args(control, original_sender,
                           std::forward<Args>(args)...);
      (control->*event_ptr)()->tunnel_.Raise(event_args);
      if (event_args.IsHandled()) {
        log += " marked as handled.";
        handled = true;
        break;
      }
    }

    // bubble
    if (!handled) {
      log += "\n\tBubble:";
      for (auto resolver : receive_list) {
        auto control = resolver.Resolve();
        log += " ";
        if (!control) {
          log += "(deleted)";
          continue;
        }
        log += control->GetDebugId();
        EventArgs event_args(control, original_sender,
                             std::forward<Args>(args)...);
        (control->*event_ptr)()->bubble_.Raise(event_args);
        if (event_args.IsHandled()) {
          log += " marked as handled.";
          break;
        }
      }
    }

    log += "\n\tDirect:";
    // direct
    for (auto resolver : receive_list) {
      auto control = resolver.Resolve();
      log += " ";
      if (!control) {
        log += "(deleted)";
        continue;
      }
      log += control->GetDebugId();
      EventArgs event_args(control, original_sender,
                           std::forward<Args>(args)...);
      (control->*event_ptr)()->direct_.Raise(event_args);
    }
  }

  void UpdateCursor();
  void NotifyControlParentChange(Control* control, Control* old_parent,
                                 Control* new_parent);

 private:
  int event_handling_count_;

  Control* root_control_;
  std::unique_ptr<platform::gui::INativeWindow> native_window_;

  /**
   * Non-nullable. At least set it to root control.
   */
  Control* focus_control_;

  /**
   * Nullable.
   */
  Control* mouse_hover_control_;

  /**
   * Nullable.
   */
  Control* mouse_captured_control_;

  std::shared_ptr<platform::gui::ICursor> override_cursor_;

  bool layout_prefer_to_fill_window_;

  Rect paint_invalid_area_;

  platform::gui::TimerAutoCanceler relayout_schedule_canceler_;
};
}  // namespace cru::ui::controls
