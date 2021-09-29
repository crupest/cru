#pragma once
#include "Resource.hpp"
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
  explicit OsxWindow(OsxUiApplication* ui_application);

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

  IEvent<std::nullptr_t>* DestroyEvent() override;
  IEvent<std::nullptr_t>* PaintEvent() override;
  IEvent<Size>* ResizeEvent() override;
  IEvent<FocusChangeType>* FocusEvent() override;
  IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() override;
  IEvent<Point>* MouseMoveEvent() override;
  IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() override;
  IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() override;
  IEvent<NativeMouseWheelEventArgs>* MouseWheelEvent() override;
  IEvent<NativeKeyEventArgs>* KeyDownEvent() override;
  IEvent<NativeKeyEventArgs>* KeyUpEvent() override;

  IInputMethodContext* GetInputMethodContext() override;

 private:
  std::unique_ptr<details::OsxWindowPrivate> p_;
};
}  // namespace cru::platform::gui::osx
