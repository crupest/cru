#pragma once
#include "Resource.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Window.hpp"

#include <memory>

namespace cru::platform::gui::osx {
namespace details {
class OsxWindowPrivate;
}

class OsxUiApplication;

class OsxWindow : public OsxGuiResource, public INativeWindow {
  friend details::OsxWindowPrivate;

 public:
  OsxWindow(OsxUiApplication* ui_application, INativeWindow* parent,
            bool frame);

  CRU_DELETE_COPY(OsxWindow)
  CRU_DELETE_MOVE(OsxWindow)

  ~OsxWindow() override;

 public:
  void Close() override;

  INativeWindow* GetParent() override;

  bool IsVisible() override;
  void SetVisible(bool is_visible) override;

  Size GetClientSize() override;
  void SetClientSize(const Size& size) override;

  Rect GetWindowRect() override;
  void SetWindowRect(const Rect& rect) override;

  Point GetMousePosition() override;

  bool CaptureMouse() override;
  bool ReleaseMouse() override;

  void SetCursor(std::shared_ptr<ICursor> cursor) override;

  void RequestRepaint() override;

  std::unique_ptr<graphics::IPainter> BeginPaint() override;

  IEvent<std::nullptr_t>* DestroyEvent() override { return &destroy_event_; }
  IEvent<std::nullptr_t>* PaintEvent() override { return &paint_event_; }
  IEvent<Size>* ResizeEvent() override { return &resize_event_; }
  IEvent<FocusChangeType>* FocusEvent() override { return &focus_event_; }
  IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() override {
    return &mouse_enter_leave_event_;
  }
  IEvent<Point>* MouseMoveEvent() override { return &mouse_move_event_; }
  IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() override {
    return &mouse_down_event_;
  }
  IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() override {
    return &mouse_up_event_;
  }
  IEvent<NativeMouseWheelEventArgs>* MouseWheelEvent() override {
    return &mouse_wheel_event_;
  }
  IEvent<NativeKeyEventArgs>* KeyDownEvent() override {
    return &key_down_event_;
  }
  IEvent<NativeKeyEventArgs>* KeyUpEvent() override { return &key_up_event_; }

  IInputMethodContext* GetInputMethodContext() override;

 private:
  void CreateWindow();

 private:
  std::unique_ptr<details::OsxWindowPrivate> p_;

  Event<std::nullptr_t> destroy_event_;
  Event<std::nullptr_t> paint_event_;
  Event<Size> resize_event_;
  Event<FocusChangeType> focus_event_;
  Event<MouseEnterLeaveType> mouse_enter_leave_event_;
  Event<Point> mouse_move_event_;
  Event<NativeMouseButtonEventArgs> mouse_down_event_;
  Event<NativeMouseButtonEventArgs> mouse_up_event_;
  Event<NativeMouseWheelEventArgs> mouse_wheel_event_;
  Event<NativeKeyEventArgs> key_down_event_;
  Event<NativeKeyEventArgs> key_up_event_;
};
}  // namespace cru::platform::gui::osx
