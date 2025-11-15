#include "cru/platform/gui/sdl/Window.h"
#include "cru/base/Base.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/NullPainter.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/sdl/Base.h"
#include "cru/platform/gui/sdl/UiApplication.h"

#include <SDL3/SDL_video.h>
#include <cairo-xcb.h>
#include <cairo.h>
#include <cassert>
#include <memory>
#include <optional>

namespace cru::platform::gui::sdl {

SdlWindow::SdlWindow(SdlUiApplication* application)
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

INativeWindow* SdlWindow::GetParent() { return parent_; }

void SdlWindow::SetParent(INativeWindow* parent) {
  parent_ = CheckPlatform<SdlWindow>(parent, GetPlatformId());
  DoSetParent(parent);
}

WindowStyleFlag SdlWindow::GetStyleFlag() { return style_; }

void SdlWindow::SetStyleFlag(WindowStyleFlag flag) {
  style_ = flag;
  DoSetStyleFlag(flag);
}

std::string SdlWindow::GetTitle() { return title_; }

void SdlWindow::SetTitle(std::string title) {
  title_ = std::move(title);
  DoSetTitle(title);
}

WindowVisibilityType SdlWindow::GetVisibility() {
  if (!sdl_window_) {
    return WindowVisibilityType::Hide;
  }

  auto flags = SDL_GetWindowFlags(*sdl_window_);
  if (flags & SDL_WINDOW_HIDDEN) {
    return WindowVisibilityType::Hide;
  } else if (flags & SDL_WINDOW_MINIMIZED) {
    return WindowVisibilityType::Minimize;
  } else {
    return WindowVisibilityType::Show;
  }
}

void SdlWindow::SetVisibility(WindowVisibilityType visibility) {
  if (visibility == WindowVisibilityType::Hide) {
    if (!sdl_window_) return;
    CheckSdlReturn(SDL_HideWindow(*sdl_window_));
  } else if (visibility == WindowVisibilityType::Minimize) {
    if (!sdl_window_) DoCreate();
    CheckSdlReturn(SDL_MinimizeWindow(*sdl_window_));
  } else {
    if (!sdl_window_) DoCreate();
    CheckSdlReturn(SDL_ShowWindow(*sdl_window_));
  }
}

Size SdlWindow::GetClientSize() { return GetClientRect().GetSize(); }

void SdlWindow::SetClientSize(const Size& size) {
  auto rect = GetClientRect();
  SetClientRect(Rect(rect.GetLeftTop(), size));
}

Rect SdlWindow::GetClientRect() {
  if (!sdl_window_) return {};
  NotImplemented();
}

void SdlWindow::SetClientRect(const Rect& rect) {
  if (!sdl_window_) return;
  NotImplemented();
}

Rect SdlWindow::GetWindowRect() {
  if (!sdl_window_) return {};
  NotImplemented();
}

void SdlWindow::SetWindowRect(const Rect& rect) {
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

IEvent<std::nullptr_t>* SdlWindow::CreateEvent() { return &create_event_; }

IEvent<std::nullptr_t>* SdlWindow::DestroyEvent() { return &destroy_event_; }

IEvent<std::nullptr_t>* SdlWindow::PaintEvent() { return &paint_event_; }

IEvent<WindowVisibilityType>* SdlWindow::VisibilityChangeEvent() {
  return &visibility_change_event_;
}

IEvent<const Size&>* SdlWindow::ResizeEvent() { return &resize_event_; }

IEvent<FocusChangeType>* SdlWindow::FocusEvent() { return &focus_event_; }

IEvent<MouseEnterLeaveType>* SdlWindow::MouseEnterLeaveEvent() {
  return &mouse_enter_leave_event_;
}

IEvent<const Point&>* SdlWindow::MouseMoveEvent() { return &mouse_move_event_; }

IEvent<const NativeMouseButtonEventArgs&>* SdlWindow::MouseDownEvent() {
  return &mouse_down_event_;
}

IEvent<const NativeMouseButtonEventArgs&>* SdlWindow::MouseUpEvent() {
  return &mouse_up_event_;
}

IEvent<const NativeMouseWheelEventArgs&>* SdlWindow::MouseWheelEvent() {
  return &mouse_wheel_event_;
}

IEvent<const NativeKeyEventArgs&>* SdlWindow::KeyDownEvent() {
  return &key_down_event_;
}

IEvent<const NativeKeyEventArgs&>* SdlWindow::KeyUpEvent() {
  return &key_up_event_;
}

IInputMethodContext* SdlWindow::GetInputMethodContext() { NotImplemented(); }

std::optional<SDL_Window*> SdlWindow::GetSdlWindow() { return sdl_window_; }

SdlUiApplication* SdlWindow::GetSdlUiApplication() { return application_; }

void SdlWindow::DoCreate() { NotImplemented(); }

void SdlWindow::DoSetParent(INativeWindow* parent) {
  if (sdl_window_) {
    CheckSdlReturn(SDL_SetWindowParent(*sdl_window_,
                                       parent_->sdl_window_.value_or(nullptr)));
  }
}

void SdlWindow::DoSetStyleFlag(WindowStyleFlag flag) {
  if (sdl_window_) {
    CheckSdlReturn(SDL_SetWindowBordered(
        *sdl_window_, !flag.Has(WindowStyleFlags::NoCaptionAndBorder)));
  }
}

void SdlWindow::DoSetTitle(const std::string& title) {
  if (sdl_window_) {
    CheckSdlReturn(SDL_SetWindowTitle(*sdl_window_, title.c_str()));
  }
}

}  // namespace cru::platform::gui::sdl
