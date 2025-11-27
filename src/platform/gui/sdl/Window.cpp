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
    : application_(application),
      client_rect_(100, 100, 400, 200),
      parent_(nullptr) {
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
  auto flags = SDL_GetWindowFlags(*sdl_window_);
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
      CheckSdlReturn(SDL_HideWindow(*sdl_window_));
    }
  } else if (visibility == WindowVisibilityType::Minimize) {
    if (!sdl_window_) {
      DoCreateWindow();
    }
    CheckSdlReturn(SDL_MinimizeWindow(*sdl_window_));
  } else {
    if (!sdl_window_) {
      DoCreateWindow();
    }
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

IInputMethodContext* SdlWindow::GetInputMethodContext() { NotImplemented(); }

std::optional<SDL_Window*> SdlWindow::GetSdlWindow() { return sdl_window_; }

SdlUiApplication* SdlWindow::GetSdlUiApplication() { return application_; }

float SdlWindow::GetDisplayScale() {
  if (!sdl_window_) return 1.f;
  auto scale = SDL_GetWindowDisplayScale(*sdl_window_);
  if (scale == 0.f) {
    throw SdlException("Failed to get window display scale.");
  }
  return scale;
}

Thickness SdlWindow::GetBorderThickness() {
  if (!sdl_window_) return {};
  int top, left, bottom, right;
  CheckSdlReturn(
      SDL_GetWindowBordersSize(*sdl_window_, &top, &left, &bottom, &right));
  return {static_cast<float>(left), static_cast<float>(top),
          static_cast<float>(right), static_cast<float>(bottom)};
}

void SdlWindow::DoCreateWindow() { NotImplemented(); }

void SdlWindow::DoUpdateClientRect() {
  assert(sdl_window_);
  CheckSdlReturn(
      SDL_SetWindowPosition(*sdl_window_, client_rect_.left, client_rect_.top));
  CheckSdlReturn(
      SDL_SetWindowSize(*sdl_window_, client_rect_.width, client_rect_.height));
  CheckSdlReturn(SDL_SyncWindow(*sdl_window_));
}

void SdlWindow::DoUpdateParent() {
  assert(sdl_window_);
  CheckSdlReturn(SDL_SetWindowParent(
      *sdl_window_,
      parent_ == nullptr ? nullptr : parent_->sdl_window_.value_or(nullptr)));
  CheckSdlReturn(SDL_SyncWindow(*sdl_window_));
}

void SdlWindow::DoUpdateStyleFlag() {
  assert(sdl_window_);
  CheckSdlReturn(SDL_SetWindowBordered(
      *sdl_window_, !style_.Has(WindowStyleFlags::NoCaptionAndBorder)));
  CheckSdlReturn(SDL_SyncWindow(*sdl_window_));
}

void SdlWindow::DoUpdateTitle() {
  assert(sdl_window_);
  CheckSdlReturn(SDL_SetWindowTitle(*sdl_window_, title_.c_str()));
  CheckSdlReturn(SDL_SyncWindow(*sdl_window_));
}

}  // namespace cru::platform::gui::sdl
