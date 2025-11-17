#pragma once
#include "../render/StackLayoutRenderObject.h"
#include "LayoutControl.h"

#include <cru/base/Base.h>
#include <cru/base/Event.h>
#include <cru/base/Guard.h>
#include <cru/base/log/Logger.h>
#include <cru/platform/gui/UiApplication.h>
#include <cru/platform/gui/Window.h>

namespace cru::ui::controls {
class CRU_UI_API Window
    : public LayoutControl<render::StackLayoutRenderObject> {
  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::controls::Window")
 public:
  static constexpr std::string_view kControlType = "Window";

  Window();
  ~Window() override;

  static Window* CreatePopup();

  std::string GetControlType() const override;

  void SetAttachedControl(Control* control);

  platform::gui::INativeWindow* GetNativeWindow();

  void InvalidateLayout();
  void InvalidatePaint();

  void Repaint();
  void Relayout();
  void RelayoutWithSize(const Size& available_size = Size::Infinite(),
                        bool set_window_size_to_fit_content = false);

  void SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value);

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
  void UpdateCursor();

  CRU_DEFINE_EVENT(AfterLayout, std::nullptr_t)

 private:
  std::unique_ptr<platform::gui::INativeWindow> CreateNativeWindow();

  void OnNativeDestroy(platform::gui::INativeWindow* window, std::nullptr_t);
  void OnNativePaint(platform::gui::INativeWindow* window, std::nullptr_t);
  void OnNativeResize(platform::gui::INativeWindow* window, const Size& size);
  void OnNativeFocus(platform::gui::INativeWindow* window,
                     cru::platform::gui::FocusChangeType focus);
  void OnNativeMouseEnterLeave(platform::gui::INativeWindow* window,
                               cru::platform::gui::MouseEnterLeaveType enter);
  void OnNativeMouseMove(platform::gui::INativeWindow* window,
                         const Point& point);
  void OnNativeMouseDown(platform::gui::INativeWindow* window,
                         const platform::gui::NativeMouseButtonEventArgs& args);
  void OnNativeMouseUp(platform::gui::INativeWindow* window,
                       const platform::gui::NativeMouseButtonEventArgs& args);
  void OnNativeMouseWheel(platform::gui::INativeWindow* window,
                          const platform::gui::NativeMouseWheelEventArgs& args);
  void OnNativeKeyDown(platform::gui::INativeWindow* window,
                       const platform::gui::NativeKeyEventArgs& args);
  void OnNativeKeyUp(platform::gui::INativeWindow* window,
                     const platform::gui::NativeKeyEventArgs& args);

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
                      (original_sender->*event_ptr)()->GetName() + ":\n\tTunnel:";

    Guard logging_guard([&] {
      log += "\nEnd dispatching routed event " +
             (original_sender->*event_ptr)()->GetName() + ".";
      CRU_LOG_TAG_DEBUG("{}", log);
    });

    std::vector<Control*> receive_list;

    auto parent = original_sender;
    while (parent != last_receiver) {
      receive_list.push_back(parent);
      parent = parent->GetParent();
    }

    auto handled = false;

    // tunnel
    for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
      auto control = *i;
      log += " ";
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
      for (auto control : receive_list) {
        log += " ";
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
    for (auto control : receive_list) {
      log += " ";
      log += control->GetDebugId();
      EventArgs event_args(control, original_sender,
                           std::forward<Args>(args)...);
      (control->*event_ptr)()->direct_.Raise(event_args);
    }
  }

 private:
  int event_handling_count_;

  std::unique_ptr<platform::gui::INativeWindow> native_window_;

  Control* focus_control_;
  Control* mouse_hover_control_;
  Control* mouse_captured_control_;

  std::shared_ptr<platform::gui::ICursor> override_cursor_;

  bool layout_prefer_to_fill_window_;

  platform::gui::TimerAutoCanceler repaint_schedule_canceler_;
  platform::gui::TimerAutoCanceler relayout_schedule_canceler_;

  Control* attached_control_;

  EventHandlerRevokerListGuard
      gain_focus_on_create_and_destroy_when_lose_focus_event_guard_;
};
}  // namespace cru::ui::controls
