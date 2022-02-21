#pragma once
#include "../Base.h"

#include "../render/RenderObject.h"
#include "cru/common/Event.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"

#include <functional>
#include <memory>
#include <optional>

namespace cru::ui::host {
class LayoutPaintCycler;

struct AfterLayoutEventArgs {};

// The bridge between control tree and native window.
class CRU_UI_API WindowHost : public Object {
  friend controls::Control;
  CRU_DEFINE_CLASS_LOG_TAG(u"WindowHost")

 public:
  explicit WindowHost(controls::Control* root_control);

  CRU_DELETE_COPY(WindowHost)
  CRU_DELETE_MOVE(WindowHost)

  ~WindowHost() override;

 public:
  platform::gui::INativeWindow* GetNativeWindow() {
    return native_window_.get();
  }

  // Mark the layout as invalid, and arrange a re-layout later.
  // This method could be called more than one times in a message cycle. But
  // layout only takes place once.
  void InvalidateLayout();

  // Mark the paint as invalid, and arrange a re-paint later.
  // This method could be called more than one times in a message cycle. But
  // paint only takes place once.
  void InvalidatePaint();

  IEvent<AfterLayoutEventArgs>* AfterLayoutEvent() {
    return &after_layout_event_;
  }

  void Relayout();
  void RelayoutWithSize(const Size& available_size = Size::Infinate(),
                        bool set_window_size_to_fit_content = false);

  void Repaint();

  // Is layout is invalid, wait for relayout and then run the action. Otherwist
  // run it right now.
  void RunAfterLayoutStable(std::function<void()> action);

  // If true, preferred size of root render object is set to window size when
  // measure. Default is true.
  bool IsLayoutPreferToFillWindow() const;
  void SetLayoutPreferToFillWindow(bool value);

  // Get current control that mouse hovers on. This ignores the mouse-capture
  // control. Even when mouse is captured by another control, this function
  // return the control under cursor. You can use `GetMouseCaptureControl` to
  // get more info.
  controls::Control* GetMouseHoverControl() const {
    return mouse_hover_control_;
  }

  //*************** region: focus ***************

  controls::Control* GetFocusControl();

  void SetFocusControl(controls::Control* control);

  //*************** region: focus ***************

  // Pass nullptr to release capture. If mouse is already capture by a control,
  // this capture will fail and return false. If control is identical to the
  // capturing control, capture is not changed and this function will return
  // true.
  //
  // When capturing control changes,
  // appropriate event will be sent. If mouse is not on the capturing control
  // and capture is released, mouse enter event will be sent to the mouse-hover
  // control. If mouse is not on the capturing control and capture is set, mouse
  // leave event will be sent to the mouse-hover control.
  bool CaptureMouseFor(controls::Control* control);

  // Return null if not captured.
  controls::Control* GetMouseCaptureControl();

  controls::Control* HitTest(const Point& point);

  void UpdateCursor();

  IEvent<platform::gui::INativeWindow*>* NativeWindowChangeEvent() {
    return &native_window_change_event_;
  }

  std::shared_ptr<platform::gui::ICursor> GetOverrideCursor();
  void SetOverrideCursor(std::shared_ptr<platform::gui::ICursor> cursor);

 private:
  std::unique_ptr<platform::gui::INativeWindow> CreateNativeWindow();

  //*************** region: native messages ***************
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

  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(controls::Control* old_control,
                                            controls::Control* new_control,
                                            const Point& point, bool no_leave,
                                            bool no_enter);

 private:
  controls::Control* root_control_ = nullptr;
  render::RenderObject* root_render_object_ = nullptr;

  std::unique_ptr<platform::gui::INativeWindow> native_window_;

  std::unique_ptr<LayoutPaintCycler> layout_paint_cycler_;

  Event<AfterLayoutEventArgs> after_layout_event_;
  std::vector<std::function<void()> > after_layout_stable_action_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  controls::Control* mouse_hover_control_ = nullptr;

  controls::Control* focus_control_;

  controls::Control* mouse_captured_control_ = nullptr;

  bool layout_prefer_to_fill_window_ = false;

  Event<platform::gui::INativeWindow*> native_window_change_event_;

  std::shared_ptr<platform::gui::ICursor> override_cursor_;
};
}  // namespace cru::ui::host
