#include "cru/platform/gui/win/UiApplication.h"

#include "TimerManager.h"
#include "WindowManager.h"
#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/gui/win/Base.h"
#include "cru/platform/gui/win/Clipboard.h"
#include "cru/platform/gui/win/Cursor.h"
#include "cru/platform/gui/win/Exception.h"
#include "cru/platform/gui/win/GodWindow.h"
#include "cru/platform/gui/win/Window.h"

namespace cru::platform::gui {
std::unique_ptr<IUiApplication> CreateUiApplication() {
  return std::make_unique<win::WinUiApplication>();
}
}  // namespace cru::platform::gui

namespace cru::platform::gui::win {
WinUiApplication* WinUiApplication::instance = nullptr;

WinUiApplication::WinUiApplication() {
  instance = this;

  instance_handle_ = ::GetModuleHandleW(nullptr);
  if (!instance_handle_)
    throw Win32Error("Failed to get module(instance) handle.");

  ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

  graph_factory_ = std::make_unique<
      cru::platform::graphics::direct2d::DirectGraphicsFactory>();

  god_window_ = std::make_unique<GodWindow>(this);
  timer_manager_ = std::make_unique<TimerManager>(god_window_.get());
  window_manager_ = std::make_unique<WindowManager>(this);
  cursor_manager_ = std::make_unique<WinCursorManager>();
  clipboard_ = std::make_unique<WinClipboard>(this);
}

WinUiApplication::~WinUiApplication() { instance = nullptr; }

int WinUiApplication::Run() {
  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
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
  return this->timer_manager_->SetTimer(TimerType::Immediate, 0,
                                        std::move(action));
}

long long WinUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                       std::function<void()> action) {
  return this->timer_manager_->SetTimer(TimerType::Timeout,
                                        static_cast<int>(milliseconds.count()),
                                        std::move(action));
}

long long WinUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  return this->timer_manager_->SetTimer(TimerType::Interval,
                                        static_cast<int>(milliseconds.count()),
                                        std::move(action));
}

void WinUiApplication::CancelTimer(long long id) {
  timer_manager_->CancelTimer(id);
}

std::vector<INativeWindow*> WinUiApplication::GetAllWindow() {
  const auto&& windows = window_manager_->GetAllWindows();
  std::vector<INativeWindow*> result;
  for (const auto w : windows) {
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

}  // namespace cru::platform::gui::win
