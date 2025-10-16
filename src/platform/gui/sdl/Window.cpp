#include "cru/platform/gui/sdl/Window.h"
#include "cru/base/Base.h"
#include "cru/platform/Check.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/NullPainter.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/sdl/UiApplication.h"

#include <SDL_video.h>
#include <cairo-xcb.h>
#include <cairo.h>
#include <cassert>
#include <memory>
#include <optional>

namespace cru::platform::gui::sdl {

SdlWindow::SdlWindow(SdlUiApplication *application)
    : application_(application), parent_(nullptr) {
  application->RegisterWindow(this);
}

SdlWindow::~SdlWindow() { application_->UnregisterWindow(this); }

bool SdlWindow::IsCreated() { return sdl_window_.has_value(); }

void SdlWindow::Close() {
  if (sdl_window_) {
    SDL_DestroyWindow(*sdl_window_);
  }
}

INativeWindow *SdlWindow::GetParent() { return parent_; }

void SdlWindow::SetParent(INativeWindow *parent) {
  parent_ = CheckPlatform<SdlWindow>(parent, GetPlatformIdUtf8());
  NotImplemented();
}

WindowStyleFlag SdlWindow::GetStyleFlag() { return style_; }

void SdlWindow::SetStyleFlag(WindowStyleFlag flag) {
  style_ = flag;
  NotImplemented();
}

String SdlWindow::GetTitle() { NotImplemented(); }

void SdlWindow::SetTitle(String title) { NotImplemented(); }

WindowVisibilityType SdlWindow::GetVisibility() { NotImplemented(); }

void SdlWindow::SetVisibility(WindowVisibilityType visibility) {
  NotImplemented();
}

Size SdlWindow::GetClientSize() { return GetClientRect().GetSize(); }

void SdlWindow::SetClientSize(const Size &size) {
  auto rect = GetClientRect();
  SetClientRect(Rect(rect.GetLeftTop(), size));
}

Rect SdlWindow::GetClientRect() {
  if (!sdl_window_) return {};
  NotImplemented();
}

void SdlWindow::SetClientRect(const Rect &rect) {
  if (!sdl_window_) return;
  NotImplemented();
}

Rect SdlWindow::GetWindowRect() {
  if (!sdl_window_) return {};
  NotImplemented();
}

void SdlWindow::SetWindowRect(const Rect &rect) {
  if (!sdl_window_) return;
  NotImplemented();
}

bool SdlWindow::RequestFocus() {
  if (!sdl_window_) return false;
  NotImplemented();
}

Point SdlWindow::GetMousePosition() { NotImplemented(); }

bool SdlWindow::CaptureMouse() {
  if (!sdl_window_) return false;
  NotImplemented();
}

bool SdlWindow::ReleaseMouse() {
  if (!sdl_window_) return false;
  NotImplemented();
}

void SdlWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  if (!sdl_window_) return;
  NotImplemented();
}

void SdlWindow::SetToForeground() {
  SetVisibility(WindowVisibilityType::Show);
  NotImplemented();
}

void SdlWindow::RequestRepaint() {
  if (!sdl_window_) return;
  NotImplemented();
}

std::unique_ptr<graphics::IPainter> SdlWindow::BeginPaint() {
  if (!sdl_window_.has_value()) {
    return std::make_unique<graphics::NullPainter>();
  }

  NotImplemented();
}

IEvent<std::nullptr_t> *SdlWindow::CreateEvent() { return &create_event_; }

IEvent<std::nullptr_t> *SdlWindow::DestroyEvent() { return &destroy_event_; }

IEvent<std::nullptr_t> *SdlWindow::PaintEvent() { return &paint_event_; }

IEvent<WindowVisibilityType> *SdlWindow::VisibilityChangeEvent() {
  return &visibility_change_event_;
}

IEvent<Size> *SdlWindow::ResizeEvent() { return &resize_event_; }

IEvent<FocusChangeType> *SdlWindow::FocusEvent() { return &focus_event_; }

IEvent<MouseEnterLeaveType> *SdlWindow::MouseEnterLeaveEvent() {
  return &mouse_enter_leave_event_;
}

IEvent<Point> *SdlWindow::MouseMoveEvent() { return &mouse_move_event_; }

IEvent<NativeMouseButtonEventArgs> *SdlWindow::MouseDownEvent() {
  return &mouse_down_event_;
}

IEvent<NativeMouseButtonEventArgs> *SdlWindow::MouseUpEvent() {
  return &mouse_up_event_;
}

IEvent<NativeMouseWheelEventArgs> *SdlWindow::MouseWheelEvent() {
  return &mouse_wheel_event_;
}

IEvent<NativeKeyEventArgs> *SdlWindow::KeyDownEvent() {
  return &key_down_event_;
}

IEvent<NativeKeyEventArgs> *SdlWindow::KeyUpEvent() { return &key_up_event_; }

IInputMethodContext *SdlWindow::GetInputMethodContext() { NotImplemented(); }

std::optional<SDL_Window *> SdlWindow::GetSdlWindow() { return sdl_window_; }

SdlUiApplication *SdlWindow::GetSdlUiApplication() { return application_; }

}  // namespace cru::platform::gui::sdl
