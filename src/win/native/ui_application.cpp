#include "cru/win/native/ui_application.hpp"

#include "../debug_logger.hpp"
#include "cru/common/logger.hpp"
#include "cru/platform/check.hpp"
#include "cru/win/graph/direct/factory.hpp"
#include "cru/win/native/cursor.hpp"
#include "cru/win/native/exception.hpp"
#include "cru/win/native/god_window.hpp"
#include "cru/win/native/window.hpp"
#include "god_window_message.hpp"
#include "timer.hpp"
#include "window_manager.hpp"

#include <cassert>

namespace cru::platform::native::win {
WinUiApplication* WinUiApplication::instance = nullptr;

WinUiApplication::WinUiApplication() {
  if (instance) {
    throw new std::runtime_error(
        "Already created an instance of WinUiApplication");
  }

  instance = this;
  IUiApplication::instance = this;

  instance_handle_ = ::GetModuleHandleW(nullptr);
  if (!instance_handle_)
    throw Win32Error("Failed to get module(instance) handle.");

  log::Logger::GetInstance()->AddSource(
      std::make_unique<::cru::platform::win::WinDebugLoggerSource>());

  graph_factory_ =
      std::make_unique<cru::platform::graph::win::direct::DirectGraphFactory>();

  god_window_ = std::make_unique<GodWindow>(this);
  timer_manager_ = std::make_unique<TimerManager>(god_window_.get());
  window_manager_ = std::make_unique<WindowManager>(this);
  cursor_manager_ = std::make_unique<WinCursorManager>();
}

WinUiApplication::~WinUiApplication() {
  IUiApplication::instance = nullptr;
  instance = nullptr;
}

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

void WinUiApplication::AddOnQuitHandler(const std::function<void()>& handler) {
  quit_handlers_.push_back(handler);
}

void WinUiApplication::InvokeLater(const std::function<void()>& action) {
  // copy the action to a safe place
  auto p_action_copy = new std::function<void()>(action);

  if (::PostMessageW(GetGodWindow()->GetHandle(), invoke_later_message_id,
                     reinterpret_cast<WPARAM>(p_action_copy), 0) == 0)
    throw Win32Error(::GetLastError(), "InvokeLater failed to post message.");
}

unsigned long WinUiApplication::SetTimeout(
    std::chrono::milliseconds milliseconds,
    const std::function<void()>& action) {
  return static_cast<unsigned long>(timer_manager_->CreateTimer(
      static_cast<UINT>(milliseconds.count()), false, action));
}

unsigned long WinUiApplication::SetInterval(
    std::chrono::milliseconds milliseconds,
    const std::function<void()>& action) {
  return static_cast<unsigned long>(timer_manager_->CreateTimer(
      static_cast<UINT>(milliseconds.count()), true, action));
}

void WinUiApplication::CancelTimer(unsigned long id) {
  timer_manager_->KillTimer(static_cast<UINT_PTR>(id));
}

std::vector<INativeWindow*> WinUiApplication::GetAllWindow() {
  const auto&& windows = window_manager_->GetAllWindows();
  std::vector<INativeWindow*> result;
  for (const auto w : windows) {
    result.push_back(static_cast<INativeWindow*>(w));
  }
  return result;
}

INativeWindow* WinUiApplication::CreateWindow(INativeWindow* parent) {
  WinNativeWindow* p = nullptr;
  if (parent != nullptr) {
    p = CheckPlatform<WinNativeWindow>(parent, GetPlatformId());
  }
  return new WinNativeWindow(this, window_manager_->GetGeneralWindowClass(),
                             WS_OVERLAPPEDWINDOW, p);
}

cru::platform::graph::IGraphFactory* WinUiApplication::GetGraphFactory() {
  return graph_factory_.get();
}

ICursorManager* WinUiApplication::GetCursorManager() {
  return cursor_manager_.get();
}
}  // namespace cru::platform::native::win
