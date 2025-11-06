#include "cru/platform/gui/win/Window.h"

#include "WindowManager.h"
#include "cru/base/StringUtil.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/graphics/NullPainter.h"
#include "cru/platform/graphics/direct2d/WindowPainter.h"
#include "cru/platform/gui/DebugFlags.h"
#include "cru/platform/gui/Input.h"
#include "cru/platform/gui/Window.h"
#include "cru/platform/gui/win/Cursor.h"
#include "cru/platform/gui/win/InputMethod.h"
#include "cru/platform/gui/win/Keyboard.h"
#include "cru/platform/gui/win/UiApplication.h"
#include "cru/platform/gui/win/WindowClass.h"

#include <windowsx.h>
#include <winuser.h>
#include <memory>

namespace cru::platform::gui::win {
namespace {
inline int DipToPixel(const float dip, const float dpi) {
  return static_cast<int>(dip * dpi / 96.0f);
}

inline float PixelToDip(const int pixel, const float dpi) {
  return static_cast<float>(pixel) * 96.0f / dpi;
}

DWORD CalcWindowStyle(WindowStyleFlag flag) {
  return flag & WindowStyleFlags::NoCaptionAndBorder ? WS_POPUP
                                                     : WS_OVERLAPPEDWINDOW;
}

Rect CalcWindowRectFromClient(const Rect& rect, WindowStyleFlag style_flag,
                              float dpi) {
  RECT r;
  r.left = DipToPixel(rect.left, dpi);
  r.top = DipToPixel(rect.top, dpi);
  r.right = DipToPixel(rect.GetRight(), dpi);
  r.bottom = DipToPixel(rect.GetBottom(), dpi);
  if (!AdjustWindowRectEx(&r, CalcWindowStyle(style_flag), FALSE, 0))
    throw Win32Error(::GetLastError(), "Failed to invoke AdjustWindowRectEx.");

  Rect result =
      Rect::FromVertices(PixelToDip(r.left, dpi), PixelToDip(r.top, dpi),
                         PixelToDip(r.right, dpi), PixelToDip(r.bottom, dpi));
  return result;
}

Rect CalcClientRectFromWindow(const Rect& rect, WindowStyleFlag style_flag,
                              float dpi) {
  RECT o{100, 100, 500, 500};
  RECT s = o;
  if (!AdjustWindowRectEx(&s, CalcWindowStyle(style_flag), FALSE, 0))
    throw Win32Error(::GetLastError(), "Failed to invoke AdjustWindowRectEx.");

  Rect result = rect;
  result.Shrink(Thickness(PixelToDip(s.left - o.left, dpi),
                          PixelToDip(o.top - s.top, dpi),
                          PixelToDip(s.right - o.right, dpi),
                          PixelToDip(s.bottom - o.bottom, dpi)));

  return result;
}
}  // namespace

WinNativeWindow::WinNativeWindow(WinUiApplication* application)
    : application_(application) {
  Expects(application);  // application can't be null.
}

WinNativeWindow::~WinNativeWindow() { Close(); }

void WinNativeWindow::Close() {
  if (hwnd_) ::DestroyWindow(hwnd_);
}

void WinNativeWindow::SetParent(INativeWindow* parent) {
  auto p = CheckPlatform<WinNativeWindow>(parent, GetPlatformId());
  parent_window_ = p;

  if (hwnd_) {
    ::SetParent(hwnd_, parent_window_->hwnd_);
  }
}

std::string WinNativeWindow::GetTitle() { return title_; }

void WinNativeWindow::SetTitle(std::string title) {
  title_ = title;

  if (hwnd_) {
    auto utf16_text = string::ToUtf16(title);
    ::SetWindowTextW(hwnd_, utf16_text.c_str());
  }
}

void WinNativeWindow::SetStyleFlag(WindowStyleFlag flag) {
  if (flag == style_flag_) return;

  style_flag_ = flag;
  if (hwnd_) {
    SetWindowLongPtrW(hwnd_, GWL_STYLE,
                      static_cast<LONG_PTR>(CalcWindowStyle(style_flag_)));
  }
}

void WinNativeWindow::SetVisibility(WindowVisibilityType visibility) {
  if (visibility == visibility_) return;
  visibility_ = visibility;

  if (!hwnd_) {
    RecreateWindow();
  }

  if (visibility == WindowVisibilityType::Show) {
    ShowWindow(hwnd_, SW_SHOWNORMAL);
  } else if (visibility == WindowVisibilityType::Hide) {
    ShowWindow(hwnd_, SW_HIDE);
  } else if (visibility == WindowVisibilityType::Minimize) {
    ShowWindow(hwnd_, SW_MINIMIZE);
  }
}

Size WinNativeWindow::GetClientSize() { return GetClientRect().GetSize(); }

void WinNativeWindow::SetClientSize(const Size& size) {
  client_rect_.SetSize(size);

  if (hwnd_) {
    RECT rect =
        DipToPixel(CalcWindowRectFromClient(client_rect_, style_flag_, dpi_));

    if (!SetWindowPos(hwnd_, nullptr, 0, 0, rect.right - rect.left,
                      rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE))
      throw Win32Error(::GetLastError(), "Failed to invoke SetWindowPos.");
  }
}

Rect WinNativeWindow::GetClientRect() { return client_rect_; }

void WinNativeWindow::SetClientRect(const Rect& rect) {
  client_rect_ = rect;

  if (hwnd_) {
    RECT r =
        DipToPixel(CalcWindowRectFromClient(client_rect_, style_flag_, dpi_));

    if (!SetWindowPos(hwnd_, nullptr, 0, 0, r.right - r.left, r.bottom - r.top,
                      SWP_NOZORDER | SWP_NOMOVE))
      throw Win32Error(::GetLastError(), "Failed to invoke SetWindowPos.");
  }
}

Rect WinNativeWindow::GetWindowRect() {
  if (hwnd_) {
    RECT rect;
    if (!::GetWindowRect(hwnd_, &rect))
      throw Win32Error(::GetLastError(), "Failed to invoke GetWindowRect.");

    return Rect::FromVertices(PixelToDip(rect.left), PixelToDip(rect.top),
                              PixelToDip(rect.right), PixelToDip(rect.bottom));
  } else {
    return CalcWindowRectFromClient(client_rect_, style_flag_, dpi_);
  }
}

void WinNativeWindow::SetWindowRect(const Rect& rect) {
  client_rect_ = CalcClientRectFromWindow(rect, style_flag_, dpi_);

  if (hwnd_) {
    if (!SetWindowPos(hwnd_, nullptr, DipToPixel(rect.left),
                      DipToPixel(rect.top), DipToPixel(rect.GetRight()),
                      DipToPixel(rect.GetBottom()), SWP_NOZORDER))
      throw Win32Error(::GetLastError(), "Failed to invoke SetWindowPos.");
  }
}

bool WinNativeWindow::RequestFocus() {
  if (hwnd_) {
    SetFocus(hwnd_);
    return true;
  }
  return false;
}

Point WinNativeWindow::GetMousePosition() {
  POINT p;
  if (!::GetCursorPos(&p))
    throw Win32Error(::GetLastError(), "Failed to get cursor position.");
  if (!::ScreenToClient(hwnd_, &p))
    throw Win32Error(::GetLastError(), "Failed to call ScreenToClient.");
  return PixelToDip(p);
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
  if constexpr (DebugFlags::paint) {
    CRU_LOG_TAG_DEBUG("A repaint is requested.");
  }
  if (!::InvalidateRect(hwnd_, nullptr, FALSE))
    throw Win32Error(::GetLastError(), "Failed to invalidate window.");
  if (!::UpdateWindow(hwnd_))
    throw Win32Error(::GetLastError(), "Failed to update window.");
}

std::unique_ptr<graphics::IPainter> WinNativeWindow::BeginPaint() {
  if (hwnd_)
    return std::make_unique<graphics::direct2d::D2DWindowPainter>(
        application_->GetDirectFactory(), window_render_target_.get());
  else
    return std::make_unique<graphics::NullPainter>();
}

void WinNativeWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  if (cursor == nullptr) {
    throw std::runtime_error("Can't use a nullptr as cursor.");
  }

  cursor_ = CheckPlatform<WinCursor>(cursor, GetPlatformId());

  if (hwnd_) return;

  if (!::SetClassLongPtrW(hwnd_, GCLP_HCURSOR,
                          reinterpret_cast<LONG_PTR>(cursor_->GetHandle()))) {
    CRU_LOG_TAG_WARN(
        "Failed to set cursor because failed to set class long. Last "
        "error code: {}.",
        ::GetLastError());
    return;
  }

  if (GetVisibility() != WindowVisibilityType::Show) return;

  auto lg = [](std::string_view reason) {
    CRU_LOG_TAG_WARN(
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

void WinNativeWindow::SetToForeground() {
  if (hwnd_) {
    if (!::SetForegroundWindow(hwnd_))
      throw Win32Error(::GetLastError(), "Failed to set window to foreground.");
  }
}

IInputMethodContext* WinNativeWindow::GetInputMethodContext() {
  return static_cast<IInputMethodContext*>(input_method_context_.get());
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
      OnMouseDownInternal(platform::gui::MouseButtons::Left, point);
      *result = 0;
      return true;
    }
    case WM_LBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::gui::MouseButtons::Left, point);
      *result = 0;
      return true;
    }
    case WM_RBUTTONDOWN: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseDownInternal(platform::gui::MouseButtons::Right, point);
      *result = 0;
      return true;
    }
    case WM_RBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::gui::MouseButtons::Right, point);
      *result = 0;
      return true;
    }
    case WM_MBUTTONDOWN: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseDownInternal(platform::gui::MouseButtons::Middle, point);
      *result = 0;
      return true;
    }
    case WM_MBUTTONUP: {
      POINT point;
      point.x = GET_X_LPARAM(l_param);
      point.y = GET_Y_LPARAM(l_param);
      OnMouseUpInternal(platform::gui::MouseButtons::Middle, point);
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
    case WM_CREATE:
      OnCreateInternal();
      *result = 0;
      return true;
    case WM_MOVE:
      OnMoveInternal(LOWORD(l_param), HIWORD(l_param));
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
    case WM_DPICHANGED: {
      dpi_ = static_cast<float>(LOWORD(w_param));
      const RECT* suggest_rect = reinterpret_cast<const RECT*>(l_param);
      window_render_target_->SetDpi(dpi_, dpi_);
      SetWindowPos(hwnd_, NULL, suggest_rect->left, suggest_rect->top,
                   suggest_rect->right - suggest_rect->left,
                   suggest_rect->bottom - suggest_rect->top,
                   SWP_NOZORDER | SWP_NOACTIVATE);
    }
    default:
      return false;
  }
}

RECT WinNativeWindow::GetClientRectPixel() {
  RECT rect;
  if (!::GetClientRect(hwnd_, &rect))
    throw Win32Error(::GetLastError(), "Failed to invoke GetClientRect.");
  return rect;
}

void WinNativeWindow::RecreateWindow() {
  const auto window_manager = application_->GetWindowManager();
  auto window_class = window_manager->GetGeneralWindowClass();

  hwnd_ = CreateWindowExW(
      0, window_class->GetName(), L"", CalcWindowStyle(style_flag_),
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      parent_window_ == nullptr ? nullptr : parent_window_->GetWindowHandle(),
      nullptr, application_->GetInstanceHandle(), nullptr);

  if (hwnd_ == nullptr)
    throw Win32Error(::GetLastError(), "Failed to create window.");

  auto dpi = ::GetDpiForWindow(hwnd_);
  if (dpi == 0)
    throw Win32Error(::GetLastError(), "Failed to get dpi of window.");
  dpi_ = static_cast<float>(dpi);
  CRU_LOG_TAG_DEBUG("Dpi of window is {}.", dpi_);

  window_manager->RegisterWindow(hwnd_, this);

  SetCursor(application_->GetCursorManager()->GetSystemCursor(
      cru::platform::gui::SystemCursorType::Arrow));

  auto utf16_title = string::ToUtf16(title_);
  ::SetWindowTextW(hwnd_, utf16_title.c_str());

  window_render_target_ =
      std::make_unique<graphics::direct2d::D2DWindowRenderTarget>(
          application_->GetDirectFactory(), hwnd_);
  window_render_target_->SetDpi(dpi_, dpi_);

  input_method_context_ = std::make_unique<WinInputMethodContext>(this);
  input_method_context_->DisableIME();
}

void WinNativeWindow::OnCreateInternal() { create_event_.Raise(nullptr); }

void WinNativeWindow::OnDestroyInternal() {
  destroy_event_.Raise(nullptr);
  application_->GetWindowManager()->UnregisterWindow(hwnd_);
  hwnd_ = nullptr;
}

void WinNativeWindow::OnPaintInternal() {
  paint_event_.Raise(nullptr);
  ValidateRect(hwnd_, nullptr);
  if constexpr (DebugFlags::paint) {
    CRU_LOG_TAG_DEBUG("A repaint is finished.");
  }
}

void WinNativeWindow::OnMoveInternal(const int new_left, const int new_top) {
  client_rect_.left = PixelToDip(new_left);
  client_rect_.top = PixelToDip(new_top);
}

void WinNativeWindow::OnResizeInternal(const int new_width,
                                       const int new_height) {
  client_rect_.width = PixelToDip(new_width);
  client_rect_.height = PixelToDip(new_height);
  if (!(new_width == 0 && new_height == 0)) {
    window_render_target_->ResizeBuffer(new_width, new_height);
    resize_event_.Raise(Size{PixelToDip(new_width), PixelToDip(new_height)});
  }
}

void WinNativeWindow::OnSetFocusInternal() {
  has_focus_ = true;
  focus_event_.Raise(FocusChangeType::Gain);
}

void WinNativeWindow::OnKillFocusInternal() {
  has_focus_ = false;
  focus_event_.Raise(FocusChangeType::Lose);
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

  mouse_move_event_.Raise(PixelToDip(point));
}

void WinNativeWindow::OnMouseLeaveInternal() {
  is_mouse_in_ = false;
  mouse_enter_leave_event_.Raise(MouseEnterLeaveType::Leave);
}

void WinNativeWindow::OnMouseDownInternal(platform::gui::MouseButton button,
                                          POINT point) {
  const auto dip_point = PixelToDip(point);
  mouse_down_event_.Raise({button, dip_point, RetrieveKeyMofifier()});
}

void WinNativeWindow::OnMouseUpInternal(platform::gui::MouseButton button,
                                        POINT point) {
  const auto dip_point = PixelToDip(point);
  mouse_up_event_.Raise({button, dip_point, RetrieveKeyMofifier()});
}

void WinNativeWindow::OnMouseWheelInternal(short delta, POINT point) {
  const auto dip_point = PixelToDip(point);
  const float d = -((float)delta / 120.f);
  mouse_wheel_event_.Raise({d, dip_point, RetrieveKeyMofifier()});
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
}  // namespace cru::platform::gui::win
