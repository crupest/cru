#include "cru/platform/gui/sdl/Window.h"
#include "cru/base/Base.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/NullPainter.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/sdl/Base.h"
#include "cru/platform/gui/sdl/Input.h"
#include "cru/platform/gui/sdl/UiApplication.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <cassert>
#include <memory>
#include <optional>

namespace cru::platform::gui::sdl {

SdlWindow::SdlWindow(SdlUiApplication* application)
    : application_(application),
      client_rect_(100, 100, 400, 200),
      parent_(nullptr) {
  application->RegisterWindow(this);
}

SdlWindow::~SdlWindow() { application_->UnregisterWindow(this); }

bool SdlWindow::IsCreated() { return sdl_window_ != nullptr; }

void SdlWindow::Close() {
  if (sdl_window_) {
    SDL_DestroyWindow(sdl_window_);
  }
}

INativeWindow* SdlWindow::GetParent() { return parent_; }

void SdlWindow::SetParent(INativeWindow* parent) {
  parent_ = CheckPlatform<SdlWindow>(parent, GetPlatformId());
  if (parent_ != nullptr) {
    parent_create_guard_.Reset(
        parent_->CreateEvent()->AddSpyOnlyHandler([this] {
          if (sdl_window_) {
            DoUpdateParent();
          }
        }));
  }
  if (sdl_window_) {
    DoUpdateParent();
  }
}

WindowStyleFlag SdlWindow::GetStyleFlag() { return style_; }

void SdlWindow::SetStyleFlag(WindowStyleFlag flag) {
  style_ = flag;
  if (sdl_window_) {
    DoUpdateStyleFlag();
  }
}

std::string SdlWindow::GetTitle() { return title_; }

void SdlWindow::SetTitle(std::string title) {
  title_ = std::move(title);
  if (sdl_window_) {
    DoUpdateTitle();
  }
}

WindowVisibilityType SdlWindow::GetVisibility() {
  if (!sdl_window_) return WindowVisibilityType::Hide;
  auto flags = SDL_GetWindowFlags(sdl_window_);
  if (flags & SDL_WINDOW_HIDDEN) {
    return WindowVisibilityType::Hide;
  }
  if (flags & SDL_WINDOW_MINIMIZED) {
    return WindowVisibilityType::Minimize;
  }
  return WindowVisibilityType::Show;
}

void SdlWindow::SetVisibility(WindowVisibilityType visibility) {
  if (visibility == WindowVisibilityType::Hide) {
    if (sdl_window_) {
      CheckSdlReturn(SDL_HideWindow(sdl_window_));
      CheckSdlReturn(SDL_SyncWindow(sdl_window_));
    }
  } else if (visibility == WindowVisibilityType::Minimize) {
    if (!sdl_window_) {
      DoCreateWindow();
    }
    CheckSdlReturn(SDL_MinimizeWindow(sdl_window_));
    CheckSdlReturn(SDL_SyncWindow(sdl_window_));
  } else {
    if (!sdl_window_) {
      DoCreateWindow();
    }
    CheckSdlReturn(SDL_ShowWindow(sdl_window_));
    CheckSdlReturn(SDL_SyncWindow(sdl_window_));
  }
}

Size SdlWindow::GetClientSize() { return GetClientRect().GetSize(); }

void SdlWindow::SetClientSize(const Size& size) {
  auto rect = GetClientRect();
  SetClientRect(Rect(rect.GetLeftTop(), size));
}

Rect SdlWindow::GetClientRect() {
  if (!sdl_window_) return {};
  return client_rect_;
}

void SdlWindow::SetClientRect(const Rect& rect) {
  client_rect_ = rect;
  if (sdl_window_) {
    DoUpdateClientRect();
  }
}

Rect SdlWindow::GetWindowRect() {
  if (!sdl_window_) return {};
  return client_rect_.Expand(GetBorderThickness());
}

void SdlWindow::SetWindowRect(const Rect& rect) {
  SetClientRect(rect.Shrink(GetBorderThickness()));
}

bool SdlWindow::RequestFocus() {
  if (!sdl_window_) return false;
  auto result = SDL_RaiseWindow(sdl_window_);
  CheckSdlReturn(SDL_SyncWindow(sdl_window_));
  return result;
}

Point SdlWindow::GetMousePosition() { NotImplemented(); }

bool SdlWindow::CaptureMouse() {
  if (!sdl_window_) return false;
  // SDL_SetWindowMouseGrab confines mouse in the window, should we really call
  // it?
  return true;
}

bool SdlWindow::ReleaseMouse() {
  if (!sdl_window_) return false;
  // SDL_SetWindowMouseGrab confines mouse in the window, should we really call
  // it?
  return true;
}

void SdlWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  if (!sdl_window_) return;
  NotImplemented();
}

void SdlWindow::SetToForeground() {
  SetVisibility(WindowVisibilityType::Show);
  CheckSdlReturn(SDL_RaiseWindow(sdl_window_));
  CheckSdlReturn(SDL_SyncWindow(sdl_window_));
}

void SdlWindow::RequestRepaint() {
  if (!sdl_window_) return;
  NotImplemented();
}

std::unique_ptr<graphics::IPainter> SdlWindow::BeginPaint() {
  if (!sdl_window_) {
    return std::make_unique<graphics::NullPainter>();
  }

  NotImplemented();
}

IInputMethodContext* SdlWindow::GetInputMethodContext() { NotImplemented(); }

std::optional<SDL_Window*> SdlWindow::GetSdlWindow() { return sdl_window_; }

SdlUiApplication* SdlWindow::GetSdlUiApplication() { return application_; }

float SdlWindow::GetDisplayScale() {
  if (!sdl_window_) return 1.f;
  auto scale = SDL_GetWindowDisplayScale(sdl_window_);
  if (scale == 0.f) {
    throw SdlException("Failed to get window display scale.");
  }
  return scale;
}

Thickness SdlWindow::GetBorderThickness() {
  if (!sdl_window_) return {};
  int top, left, bottom, right;
  CheckSdlReturn(
      SDL_GetWindowBordersSize(sdl_window_, &top, &left, &bottom, &right));
  return {static_cast<float>(left), static_cast<float>(top),
          static_cast<float>(right), static_cast<float>(bottom)};
}

void SdlWindow::DoCreateWindow() {
  assert(!sdl_window_);
  SDL_WindowFlags flags = SDL_WINDOW_HIDDEN;
  if (style_.Has(WindowStyleFlags::NoCaptionAndBorder)) {
    flags |= SDL_WINDOW_BORDERLESS;
  }

  sdl_window_ = SDL_CreateWindow(title_.c_str(), client_rect_.width,
                                 client_rect_.height, flags);

  if (!sdl_window_) {
    throw SdlException("Failed to create window.");
  }

  CreateEvent_.Raise(nullptr);

  CheckSdlReturn(
      SDL_SetWindowPosition(sdl_window_, client_rect_.left, client_rect_.top));
  CheckSdlReturn(SDL_SetWindowParent(
      sdl_window_, parent_ == nullptr ? nullptr : parent_->sdl_window_));
  CheckSdlReturn(SDL_SyncWindow(sdl_window_));
}

void SdlWindow::DoUpdateClientRect() {
  assert(sdl_window_);
  CheckSdlReturn(
      SDL_SetWindowPosition(sdl_window_, client_rect_.left, client_rect_.top));
  CheckSdlReturn(
      SDL_SetWindowSize(sdl_window_, client_rect_.width, client_rect_.height));
  CheckSdlReturn(SDL_SyncWindow(sdl_window_));
}

void SdlWindow::DoUpdateParent() {
  assert(sdl_window_);
  CheckSdlReturn(SDL_SetWindowParent(
      sdl_window_, parent_ == nullptr ? nullptr : parent_->sdl_window_));
  CheckSdlReturn(SDL_SyncWindow(sdl_window_));
}

void SdlWindow::DoUpdateStyleFlag() {
  assert(sdl_window_);
  CheckSdlReturn(SDL_SetWindowBordered(
      sdl_window_, !style_.Has(WindowStyleFlags::NoCaptionAndBorder)));
  CheckSdlReturn(SDL_SyncWindow(sdl_window_));
}

void SdlWindow::DoUpdateTitle() {
  assert(sdl_window_);
  CheckSdlReturn(SDL_SetWindowTitle(sdl_window_, title_.c_str()));
  CheckSdlReturn(SDL_SyncWindow(sdl_window_));
}

namespace {
NativeMouseButtonEventArgs ConvertMouseButtonEvent(
    const SDL_MouseButtonEvent& event) {
  return {
      ConvertMouseButton(event.button), {event.x, event.y}, GetKeyModifier()};
}

NativeKeyEventArgs ConvertKeyEvent(const SDL_KeyboardEvent& event) {
  return {ConvertKeyCode(event.key), ConvertKeyModifier(event.mod)};
}
}  // namespace

bool SdlWindow::HandleEvent(const SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_WINDOW_MOVED: {
      client_rect_.left = event->window.data1;
      client_rect_.top = event->window.data2;
      return true;
    }
    case SDL_EVENT_WINDOW_RESIZED: {
      client_rect_.width = event->window.data1;
      client_rect_.height = event->window.data2;
      ResizeEvent_.Raise(client_rect_.GetSize());
      return true;
    }
    case SDL_EVENT_WINDOW_SHOWN: {
      VisibilityChangeEvent_.Raise(WindowVisibilityType::Show);
      return true;
    }
    case SDL_EVENT_WINDOW_HIDDEN: {
      VisibilityChangeEvent_.Raise(WindowVisibilityType::Hide);
      return true;
    }
    case SDL_EVENT_WINDOW_MINIMIZED: {
      VisibilityChangeEvent_.Raise(WindowVisibilityType::Minimize);
      return true;
    }
    case SDL_EVENT_WINDOW_FOCUS_GAINED: {
      FocusEvent_.Raise(FocusChangeType::Gain);
      return true;
    }
    case SDL_EVENT_WINDOW_FOCUS_LOST: {
      FocusEvent_.Raise(FocusChangeType::Lose);
      return true;
    }
    case SDL_EVENT_WINDOW_MOUSE_ENTER: {
      MouseEnterLeaveEvent_.Raise(MouseEnterLeaveType::Enter);
      return true;
    }
    case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
      MouseEnterLeaveEvent_.Raise(MouseEnterLeaveType::Leave);
      return true;
    }
    case SDL_EVENT_WINDOW_DESTROYED: {
      VisibilityChangeEvent_.Raise(WindowVisibilityType::Hide);
      DestroyEvent_.Raise(nullptr);
      sdl_window_ = nullptr;
      return true;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      MouseDownEvent_.Raise(ConvertMouseButtonEvent(event->button));
      return true;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      MouseUpEvent_.Raise(ConvertMouseButtonEvent(event->button));
      return true;
    }
    case SDL_EVENT_MOUSE_WHEEL: {
      const auto& we = event->wheel;
      if (we.x != 0) {
        MouseWheelEvent_.Raise(
            {we.x, {we.mouse_x, we.mouse_y}, GetKeyModifier(), true});
      }
      if (we.y != 0) {
        MouseWheelEvent_.Raise(
            {we.y, {we.mouse_x, we.mouse_y}, GetKeyModifier(), false});
      }
      return true;
    }
    case SDL_EVENT_KEY_DOWN: {
      KeyDownEvent_.Raise(ConvertKeyEvent(event->key));
      return true;
    }
    case SDL_EVENT_KEY_UP: {
      KeyUpEvent_.Raise(ConvertKeyEvent(event->key));
      return true;
    }
  }
  return false;
}

}  // namespace cru::platform::gui::sdl
