#include "cru/win/native/win_native_window.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/graph_manager.hpp"
#include "cru/win/native/win_application.hpp"
#include "cru/win/native/window_class.hpp"
#include "cru/win/native/window_render_target.hpp"
#include "dpi_util.hpp"
#include "window_manager.hpp"
#include "window_painter.hpp"

#include <assert.h>
#include <windowsx.h>

namespace cru::win::native {
WinNativeWindow::WinNativeWindow(WinApplication* application,
                                 std::shared_ptr<WindowClass> window_class,
                                 DWORD window_style, WinNativeWindow* parent) {
  assert(application);  // application can't be null.
  assert(parent == nullptr ||
         parent->IsValid());  // Parent window is not valid.

  application_ = application;
  parent_window_ = parent;

  const auto window_manager = application->GetWindowManager();

  hwnd_ = CreateWindowExW(
      0, window_manager->GetGeneralWindowClass()->GetName(), L"", window_style,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      parent == nullptr ? nullptr : parent->GetWindowHandle(), nullptr,
      application->GetInstanceHandle(), nullptr);

  if (hwnd_ == nullptr)
    throw Win32Error(::GetLastError(), "Failed to create window.");

  window_manager->RegisterWindow(hwnd_, this);

  window_render_target_.reset(
      new WindowRenderTarget(graph::GraphManager::GetInstance(), hwnd_));
}

WinNativeWindow::~WinNativeWindow() {
  if (IsValid()) {
    SetDeleteThisOnDestroy(false);  // avoid double delete.
    Close();
  }
}

bool WinNativeWindow::IsValid() { return hwnd_ != nullptr; }

void WinNativeWindow::SetDeleteThisOnDestroy(bool value) {
  delete_this_on_destroy_ = value;
}

void WinNativeWindow::Close() {
  if (IsValid()) DestroyWindow(hwnd_);
}

bool WinNativeWindow::IsVisible() {
  if (IsValid()) return ::IsWindowVisible(hwnd_);
  return false;
}

void WinNativeWindow::SetVisible(bool is_visible) {
  if (!IsValid()) return;
  is_visible ? ShowWindow(hwnd_, SW_SHOWNORMAL) : ShowWindow(hwnd_, SW_HIDE);
}
ui::Size WinNativeWindow::GetClientSize() {
  if (!IsValid()) return ui::Size{};

  const auto pixel_rect = GetClientRectPixel();
  return ui::Size(PixelToDipX(pixel_rect.right),
                  PixelToDipY(pixel_rect.bottom));
}

void WinNativeWindow::SetClientSize(const ui::Size& size) {
  if (IsValid()) {
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
      throw Win32Error(::GetLastError(),
                       "Failed to invoke AdjustWindowRectEx.");

    if (!SetWindowPos(hwnd_, nullptr, 0, 0, rect.right - rect.left,
                      rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE))
      throw Win32Error(::GetLastError(), "Failed to invoke SetWindowPos.");
  }
}

ui::Rect WinNativeWindow::GetWindowRect() {
  if (!IsValid()) return ui::Rect{};

  RECT rect;
  if (!::GetWindowRect(hwnd_, &rect))
    throw Win32Error(::GetLastError(), "Failed to invoke GetWindowRect.");

  return ui::Rect::FromVertices(PixelToDipX(rect.left), PixelToDipY(rect.top),
                                PixelToDipX(rect.right),
                                PixelToDipY(rect.bottom));
}

void WinNativeWindow::SetWindowRect(const ui::Rect& rect) {
  if (IsValid()) {
    if (!SetWindowPos(hwnd_, nullptr, DipToPixelX(rect.left),
                      DipToPixelY(rect.top), DipToPixelX(rect.GetRight()),
                      DipToPixelY(rect.GetBottom()), SWP_NOZORDER))
      throw Win32Error(::GetLastError(), "Failed to invoke SetWindowPos.");
  }
}

platform::graph::Painter* WinNativeWindow::BeginPaint() {
  return new WindowPainter(this);
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
      OnMouseDownInternal(platform::native::MouseButton::Left, point);
      *result = 0;
      return true;
    }
    case WM_LBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::native::MouseButton::Left, point);
      *result = 0;
      return true;
    }
    case WM_RBUTTONDOWN: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseDownInternal(platform::native::MouseButton::Right, point);
      *result = 0;
      return true;
    }
    case WM_RBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::native::MouseButton::Right, point);
      *result = 0;
      return true;
    }
    case WM_MBUTTONDOWN: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseDownInternal(platform::native::MouseButton::Middle, point);
      *result = 0;
      return true;
    }
    case WM_MBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::native::MouseButton::Middle, point);
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
    case WM_CHAR:
      OnCharInternal(static_cast<wchar_t>(w_param));
      *result = 0;
      return true;
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
  destroy_event_.Raise();
  if (delete_this_on_destroy_)
    application_->InvokeLater([this] { delete this; });
}

void WinNativeWindow::OnPaintInternal() {
  paint_event_.Raise();
  ValidateRect(hwnd_, nullptr);
}

void WinNativeWindow::OnResizeInternal(const int new_width,
                                       const int new_height) {
  if (!(new_width == 0 && new_height == 0)) {
    window_render_target_->ResizeBuffer(new_width, new_height);
    resize_event_.Raise(
        ui::Size{PixelToDipX(new_width), PixelToDipY(new_height)});
  }
}

void WinNativeWindow::OnSetFocusInternal() {
  has_focus_ = true;
  focus_event_.Raise(true);
}

void WinNativeWindow::OnKillFocusInternal() {
  has_focus_ = false;
  focus_event_.Raise(false);
}

inline ui::Point PiToDip(const POINT& pi_point) {
  return ui::Point(PixelToDipX(pi_point.x), PixelToDipY(pi_point.y));
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
    mouse_enter_leave_event_.Raise(true);
  }

  const auto dip_point = PiToDip(point);
  mouse_move_event_.Raise(dip_point);
}

void WinNativeWindow::OnMouseLeaveInternal() {
  is_mouse_in_ = false;
  mouse_enter_leave_event_.Raise(false);
}

void WinNativeWindow::OnMouseDownInternal(platform::native::MouseButton button,
                                          POINT point) {
  const auto dip_point = PiToDip(point);
  mouse_down_event_.Raise(button, dip_point);
}

void WinNativeWindow::OnMouseUpInternal(platform::native::MouseButton button,
                                        POINT point) {
  const auto dip_point = PiToDip(point);
  mouse_up_event_.Raise(button, dip_point);
}

void WinNativeWindow::OnMouseWheelInternal(short delta, POINT point) {}

void WinNativeWindow::OnKeyDownInternal(int virtual_code) {
  key_down_event_.Raise(virtual_code);
}

void WinNativeWindow::OnKeyUpInternal(int virtual_code) {
  key_up_event_.Raise(virtual_code);
}

void WinNativeWindow::OnCharInternal(wchar_t c) {}

void WinNativeWindow::OnActivatedInternal() {}

void WinNativeWindow::OnDeactivatedInternal() {}
}  // namespace cru::win::native
