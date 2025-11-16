#include "cru/platform/gui/win/UiApplication.h"

#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/gui/win/Base.h"
#include "cru/platform/gui/win/Clipboard.h"
#include "cru/platform/gui/win/Cursor.h"
#include "cru/platform/gui/win/Window.h"

#include <algorithm>
#include <chrono>

namespace cru::platform::gui::win {
namespace {
LRESULT __stdcall GeneralWndProc(HWND hWnd, UINT Msg, WPARAM wParam,
                                 LPARAM lParam) {
  auto window = WinUiApplication::GetInstance()->FromHWND(hWnd);

  LRESULT result;
  if (window != nullptr &&
      window->HandleNativeWindowMessage(hWnd, Msg, wParam, lParam, &result))
    return result;

  return DefWindowProc(hWnd, Msg, wParam, lParam);
}
}  // namespace

WinUiApplication* WinUiApplication::instance = nullptr;

WinUiApplication::WinUiApplication() {
  instance = this;

  instance_handle_ = ::GetModuleHandleW(nullptr);
  if (!instance_handle_)
    throw Win32Error("Failed to get module(instance) handle.");

  ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

  general_window_class_ = std::make_unique<WindowClass>(
      L"CruUIWindowClass", GeneralWndProc, instance_handle_);

  graph_factory_ = std::make_unique<
      cru::platform::graphics::direct2d::DirectGraphicsFactory>();

  cursor_manager_ = std::make_unique<WinCursorManager>();
  clipboard_ = std::make_unique<WinClipboard>(this);
}

WinUiApplication::~WinUiApplication() { instance = nullptr; }

int WinUiApplication::Run() {
  MSG msg;
  bool exit = false;

  while (!exit) {
    if (auto result = timers_.Update(std::chrono::steady_clock::now())) {
      result->data();
      continue;
    }

    auto timeout = timers_.NextTimeout(std::chrono::steady_clock::now());

    ::MsgWaitForMultipleObjects(
        0, nullptr, FALSE, timeout ? timeout->count() : INFINITE, QS_ALLINPUT);

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);

      if (msg.message == WM_QUIT) {
        exit = true;
        break;
      }
    }
  }

  for (const auto& handler : quit_handlers_) handler();

  return static_cast<int>(msg.wParam);
}

void WinUiApplication::RequestQuit(const int quit_code) {
  ::PostQuitMessage(quit_code);
}

void WinUiApplication::AddOnQuitHandler(std::function<void()> handler) {
  quit_handlers_.push_back(std::move(handler));
}

long long WinUiApplication::SetImmediate(std::function<void()> action) {
  return timers_.Add(std::move(action), std::chrono::milliseconds::zero(),
                     false);
}

long long WinUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                       std::function<void()> action) {
  return timers_.Add(std::move(action), milliseconds, false);
}

long long WinUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  return timers_.Add(std::move(action), milliseconds, true);
}

void WinUiApplication::CancelTimer(long long id) { timers_.Remove(id); }

std::vector<INativeWindow*> WinUiApplication::GetAllWindow() {
  std::vector<INativeWindow*> result;
  for (const auto w : windows_) {
    result.push_back(static_cast<INativeWindow*>(w));
  }
  return result;
}

INativeWindow* WinUiApplication::CreateWindow() {
  return new WinNativeWindow(this);
}

cru::platform::graphics::IGraphicsFactory*
WinUiApplication::GetGraphicsFactory() {
  return graph_factory_.get();
}

ICursorManager* WinUiApplication::GetCursorManager() {
  return cursor_manager_.get();
}

IClipboard* WinUiApplication::GetClipboard() { return clipboard_.get(); }

std::vector<WinNativeWindow*> WinUiApplication::GetAllWinWindow() {
  return windows_;
}

WinNativeWindow* WinUiApplication::FromHWND(HWND hwnd) {
  for (auto window : windows_) {
    if (window->GetWindowHandle() == hwnd) {
      return window;
    }
  }
  return nullptr;
}

WindowClass* WinUiApplication::GetGeneralWindowClass() {
  return general_window_class_.get();
}

void WinUiApplication::RegisterWindow(WinNativeWindow* window) {
  windows_.push_back(window);
}

void WinUiApplication::UnregisterWindow(WinNativeWindow* window) {
  windows_.erase(std::ranges::find(windows_, window));
}
}  // namespace cru::platform::gui::win
