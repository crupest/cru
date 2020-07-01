#include "cru/win/native/Window.hpp"

#include "DpiUtil.hpp"
#include "WindowD2DPainter.hpp"
#include "WindowManager.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/Check.hpp"
#include "cru/win/String.hpp"
#include "cru/win/native/Cursor.hpp"
#include "cru/win/native/Exception.hpp"
#include "cru/win/native/Keyboard.hpp"
#include "cru/win/native/UiApplication.hpp"
#include "cru/win/native/WindowClass.hpp"
#include "cru/win/native/WindowRenderTarget.hpp"

#include <imm.h>
#include <windowsx.h>

namespace cru::platform::native::win {
WinNativeWindow::WinNativeWindow(WinUiApplication* application,
                                 WindowClass* window_class, DWORD window_style,
                                 WinNativeWindow* parent)
    : application_(application),
      resolver_(std::make_shared<WinNativeWindowResolver>(this)),
      parent_window_(parent) {
  Expects(application);  // application can't be null.

  if (parent != nullptr) {
    throw new std::runtime_error("Can't use a invalid window as parent.");
  }

  const auto window_manager = application->GetWindowManager();

  hwnd_ = CreateWindowExW(
      0, window_class->GetName(), L"", window_style, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      parent == nullptr ? nullptr : parent->GetWindowHandle(), nullptr,
      application->GetInstanceHandle(), nullptr);

  if (hwnd_ == nullptr)
    throw Win32Error(::GetLastError(), "Failed to create window.");

  window_manager->RegisterWindow(hwnd_, this);

  SetCursor(application->GetCursorManager()->GetSystemCursor(
      cru::platform::native::SystemCursorType::Arrow));

  window_render_target_ = std::make_unique<WindowRenderTarget>(
      application->GetDirectFactory(), hwnd_);
}

WinNativeWindow::~WinNativeWindow() {
  if (!sync_flag_) {
    sync_flag_ = true;
    Close();
  }
  resolver_->Reset();
}

void WinNativeWindow::Close() { ::DestroyWindow(hwnd_); }

bool WinNativeWindow::IsVisible() { return ::IsWindowVisible(hwnd_); }

void WinNativeWindow::SetVisible(bool is_visible) {
  is_visible ? ShowWindow(hwnd_, SW_SHOWNORMAL) : ShowWindow(hwnd_, SW_HIDE);
}
Size WinNativeWindow::GetClientSize() {
  const auto pixel_rect = GetClientRectPixel();
  return Size(PixelToDipX(pixel_rect.right), PixelToDipY(pixel_rect.bottom));
}

void WinNativeWindow::SetClientSize(const Size& size) {
  const auto window_style =
      static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
  const auto window_ex_style =
      static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));

  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = DipToPixelX(size.width);
  rect.bottom = DipToPixelY(size.height);
  if (!AdjustWindowRectEx(&rect, window_style, FALSE, window_ex_style))
    throw Win32Error(::GetLastError(), "Failed to invoke AdjustWindowRectEx.");

  if (!SetWindowPos(hwnd_, nullptr, 0, 0, rect.right - rect.left,
                    rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE))
    throw Win32Error(::GetLastError(), "Failed to invoke SetWindowPos.");
}

Rect WinNativeWindow::GetWindowRect() {
  RECT rect;
  if (!::GetWindowRect(hwnd_, &rect))
    throw Win32Error(::GetLastError(), "Failed to invoke GetWindowRect.");

  return Rect::FromVertices(PixelToDipX(rect.left), PixelToDipY(rect.top),
                            PixelToDipX(rect.right), PixelToDipY(rect.bottom));
}

void WinNativeWindow::SetWindowRect(const Rect& rect) {
  if (!SetWindowPos(hwnd_, nullptr, DipToPixelX(rect.left),
                    DipToPixelY(rect.top), DipToPixelX(rect.GetRight()),
                    DipToPixelY(rect.GetBottom()), SWP_NOZORDER))
    throw Win32Error(::GetLastError(), "Failed to invoke SetWindowPos.");
}

Point WinNativeWindow::GetMousePosition() {
  POINT p;
  if (!::GetCursorPos(&p))
    throw Win32Error(::GetLastError(), "Failed to get cursor position.");
  if (!::ScreenToClient(hwnd_, &p))
    throw Win32Error(::GetLastError(), "Failed to call ScreenToClient.");
  return PiToDip(p);
}

bool WinNativeWindow::CaptureMouse() {
  ::SetCapture(hwnd_);
  return true;
}

bool WinNativeWindow::ReleaseMouse() {
  const auto result = ::ReleaseCapture();
  return result != 0;
}

void WinNativeWindow::RequestRepaint() {
  log::TagDebug(log_tag, "A repaint is requested.");
  if (!::InvalidateRect(hwnd_, nullptr, FALSE))
    throw Win32Error(::GetLastError(), "Failed to invalidate window.");
  if (!::UpdateWindow(hwnd_))
    throw Win32Error(::GetLastError(), "Failed to update window.");
}

std::unique_ptr<graph::IPainter> WinNativeWindow::BeginPaint() {
  return std::make_unique<WindowD2DPainter>(window_render_target_.get());
}

void WinNativeWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  if (cursor == nullptr) {
    throw std::runtime_error("Can't use a nullptr as cursor.");
  }

  cursor_ = CheckPlatform<WinCursor>(cursor, GetPlatformId());

  if (!::SetClassLongPtrW(hwnd_, GCLP_HCURSOR,
                          reinterpret_cast<LONG_PTR>(cursor_->GetHandle()))) {
    log::TagWarn(log_tag,
                 "Failed to set cursor because failed to set class long. Last "
                 "error code: {}.",
                 ::GetLastError());
    return;
  }

  if (!IsVisible()) return;

  auto lg = [](const std::string_view& reason) {
    log::TagWarn(
        log_tag,
        "Failed to set cursor because {} when window is visible. (We need to "
        "update cursor if it is inside the window.) Last error code: {}.",
        reason, ::GetLastError());
  };

  ::POINT point;
  if (!::GetCursorPos(&point)) {
    lg("failed to get cursor pos");
    return;
  }

  ::RECT rect;
  if (!::GetClientRect(hwnd_, &rect)) {
    lg("failed to get window's client rect");
    return;
  }

  ::POINT lefttop{rect.left, rect.top};
  ::POINT rightbottom{rect.right, rect.bottom};
  if (!::ClientToScreen(hwnd_, &lefttop)) {
    lg("failed to call ClientToScreen on lefttop");
    return;
  }

  if (!::ClientToScreen(hwnd_, &rightbottom)) {
    lg("failed to call ClientToScreen on rightbottom");
    return;
  }

  if (point.x >= lefttop.x && point.y >= lefttop.y &&
      point.x <= rightbottom.x && point.y <= rightbottom.y) {
    ::SetCursor(cursor_->GetHandle());
  }
}

bool WinNativeWindow::HandleNativeWindowMessage(HWND hwnd, UINT msg,
                                                WPARAM w_param, LPARAM l_param,
                                                LRESULT* result) {
  WindowNativeMessageEventArgs args{
      WindowNativeMessage{hwnd, msg, w_param, l_param}};
  native_message_event_.Raise(args);
  if (args.IsHandled()) {
    *result = args.GetResult();
    return true;
  }

  switch (msg) {
    case WM_PAINT:
      OnPaintInternal();
      *result = 0;
      return true;
    case WM_ERASEBKGND:
      *result = 1;
      return true;
    case WM_SETFOCUS:
      OnSetFocusInternal();
      *result = 0;
      return true;
    case WM_KILLFOCUS:
      OnKillFocusInternal();
      *result = 0;
      return true;
    case WM_MOUSEMOVE: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseMoveInternal(point);
      *result = 0;
      return true;
    }
    case WM_LBUTTONDOWN: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseDownInternal(platform::native::mouse_buttons::left, point);
      *result = 0;
      return true;
    }
    case WM_LBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::native::mouse_buttons::left, point);
      *result = 0;
      return true;
    }
    case WM_RBUTTONDOWN: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseDownInternal(platform::native::mouse_buttons::right, point);
      *result = 0;
      return true;
    }
    case WM_RBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::native::mouse_buttons::right, point);
      *result = 0;
      return true;
    }
    case WM_MBUTTONDOWN: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseDownInternal(platform::native::mouse_buttons::middle, point);
      *result = 0;
      return true;
    }
    case WM_MBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::native::mouse_buttons::middle, point);
      *result = 0;
      return true;
    }
    case WM_MOUSEWHEEL:
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      ScreenToClient(hwnd, &point);
      OnMouseWheelInternal(GET_WHEEL_DELTA_WPARAM(w_param), point);
      *result = 0;
      return true;
    case WM_KEYDOWN:
      OnKeyDownInternal(static_cast<int>(w_param));
      *result = 0;
      return true;
    case WM_KEYUP:
      OnKeyUpInternal(static_cast<int>(w_param));
      *result = 0;
      return true;
    case WM_SYSKEYDOWN:
      if (l_param & (1 << 29)) {
        OnKeyDownInternal(static_cast<int>(w_param));
        *result = 0;
        return true;
      }
      return false;
    case WM_SYSKEYUP:
      if (l_param & (1 << 29)) {
        OnKeyUpInternal(static_cast<int>(w_param));
        *result = 0;
        return true;
      }
      return false;
    case WM_SIZE:
      OnResizeInternal(LOWORD(l_param), HIWORD(l_param));
      *result = 0;
      return true;
    case WM_ACTIVATE:
      if (w_param == WA_ACTIVE || w_param == WA_CLICKACTIVE)
        OnActivatedInternal();
      else if (w_param == WA_INACTIVE)
        OnDeactivatedInternal();
      *result = 0;
      return true;
    case WM_DESTROY:
      OnDestroyInternal();
      *result = 0;
      return true;
    case WM_IME_SETCONTEXT:
      l_param &= ~ISC_SHOWUICOMPOSITIONWINDOW;
      *result = ::DefWindowProcW(hwnd, msg, w_param, l_param);
      return true;
    // We must block these message from DefWindowProc or it will create
    // an ugly composition window.
    case WM_IME_STARTCOMPOSITION:
    case WM_IME_COMPOSITION:
      *result = 0;
      return true;
    default:
      return false;
  }
}

RECT WinNativeWindow::GetClientRectPixel() {
  RECT rect;
  if (!GetClientRect(hwnd_, &rect))
    throw Win32Error(::GetLastError(), "Failed to invoke GetClientRect.");
  return rect;
}

void WinNativeWindow::OnDestroyInternal() {
  application_->GetWindowManager()->UnregisterWindow(hwnd_);
  hwnd_ = nullptr;
  destroy_event_.Raise(nullptr);
  if (!sync_flag_) {
    sync_flag_ = true;
    delete this;
  }
}

void WinNativeWindow::OnPaintInternal() {
  paint_event_.Raise(nullptr);
  ValidateRect(hwnd_, nullptr);
  log::TagDebug(log_tag, "A repaint is finished.");
}

void WinNativeWindow::OnResizeInternal(const int new_width,
                                       const int new_height) {
  if (!(new_width == 0 && new_height == 0)) {
    window_render_target_->ResizeBuffer(new_width, new_height);
    resize_event_.Raise(Size{PixelToDipX(new_width), PixelToDipY(new_height)});
  }
}

void WinNativeWindow::OnSetFocusInternal() {
  has_focus_ = true;
  focus_event_.Raise(FocusChangeType::Gain);
}

void WinNativeWindow::OnKillFocusInternal() {
  has_focus_ = false;
  focus_event_.Raise(FocusChangeType::Lost);
}

void WinNativeWindow::OnMouseMoveInternal(const POINT point) {
  // when mouse was previous outside the window
  if (!is_mouse_in_) {
    // invoke TrackMouseEvent to have WM_MOUSELEAVE sent.
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof tme;
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hwnd_;

    TrackMouseEvent(&tme);

    is_mouse_in_ = true;
    mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Enter);
  }

  mouse_move_event_.Raise(PiToDip(point));
}

void WinNativeWindow::OnMouseLeaveInternal() {
  is_mouse_in_ = false;
  mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Leave);
}

void WinNativeWindow::OnMouseDownInternal(platform::native::MouseButton button,
                                          POINT point) {
  const auto dip_point = PiToDip(point);
  mouse_down_event_.Raise({button, dip_point, RetrieveKeyMofifier()});
}

void WinNativeWindow::OnMouseUpInternal(platform::native::MouseButton button,
                                        POINT point) {
  const auto dip_point = PiToDip(point);
  mouse_up_event_.Raise({button, dip_point, RetrieveKeyMofifier()});
}

void WinNativeWindow::OnMouseWheelInternal(short delta, POINT point) {
  CRU_UNUSED(delta)
  CRU_UNUSED(point)
}

void WinNativeWindow::OnKeyDownInternal(int virtual_code) {
  key_down_event_.Raise(
      {VirtualKeyToKeyCode(virtual_code), RetrieveKeyMofifier()});
}

void WinNativeWindow::OnKeyUpInternal(int virtual_code) {
  key_up_event_.Raise(
      {VirtualKeyToKeyCode(virtual_code), RetrieveKeyMofifier()});
}

void WinNativeWindow::OnActivatedInternal() {}

void WinNativeWindow::OnDeactivatedInternal() {}

void WinNativeWindowResolver::Reset() {
  Expects(window_);  // already reset, can't reset again
  window_ = nullptr;
}

WinNativeWindow* Resolve(gsl::not_null<INativeWindowResolver*> resolver) {
  const auto window = resolver->Resolve();
  return window == nullptr ? nullptr
                           : CheckPlatform<WinNativeWindow>(
                                 window, WinNativeResource::k_platform_id);
}  // namespace cru::platform::native::win
}  // namespace cru::platform::native::win