#include "cru/platform/win/win_application.hpp"

#include "cru/platform/win/exception.hpp"
#include "cru/platform/win/god_window.hpp"
#include "cru/platform/win/graph_manager.hpp"
#include "cru/platform/win/win_graph_factory.hpp"
#include "god_window_message.hpp"
#include "timer.hpp"
#include "window_manager.hpp"

#include <VersionHelpers.h>

namespace cru::platform {
UiApplication* UiApplication::GetInstance() {
  return win::WinApplication::GetInstance();
}
}  // namespace cru::platform

namespace cru::platform::win {
WinApplication* WinApplication::instance_ = nullptr;

WinApplication* WinApplication::GetInstance() {
  if (instance_ == nullptr)
    instance_ = new WinApplication(::GetModuleHandleW(nullptr));
  return instance_;
}

WinApplication::WinApplication(HINSTANCE h_instance) : h_instance_(h_instance) {
  if (instance_)
    throw std::runtime_error("A application instance already exists.");

  instance_ = this;

  if (!::IsWindows8OrGreater())
    throw std::runtime_error("Must run on Windows 8 or later.");

  god_window_ = std::make_shared<GodWindow>(this);
  timer_manager_ = std::make_shared<TimerManager>(god_window_.get());
  window_manager_ = std::make_shared<WindowManager>(this);
  graph_manager_ = std::make_shared<GraphManager>();
  graph_factory_ = std::make_shared<GraphFactory>(graph_manager_.get());
}

WinApplication::~WinApplication() { instance_ = nullptr; }

int WinApplication::Run() {
  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  return static_cast<int>(msg.wParam);
}

void WinApplication::Quit(const int quit_code) { ::PostQuitMessage(quit_code); }

void WinApplication::InvokeLater(const std::function<void()>& action) {
  // copy the action to a safe place
  auto p_action_copy = new std::function<void()>(action);

  if (PostMessageW(GetGodWindow()->GetHandle(), invoke_later_message_id,
                   reinterpret_cast<WPARAM>(p_action_copy), 0) == 0)
    throw Win32Error(::GetLastError(), "InvokeLater failed to post message.");
}

unsigned long WinApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                         const std::function<void()>& action) {
  return static_cast<unsigned long>(timer_manager_->CreateTimer(
      static_cast<UINT>(milliseconds.count()), false, action));
}

unsigned long WinApplication::SetInterval(
    std::chrono::milliseconds milliseconds,
    const std::function<void()>& action) {
  return static_cast<unsigned long>(timer_manager_->CreateTimer(
      static_cast<UINT>(milliseconds.count()), true, action));
}

void WinApplication::CancelTimer(unsigned long id) {
  timer_manager_->KillTimer(static_cast<UINT_PTR>(id));
}
GraphFactory* WinApplication::GetGraphFactory() { return graph_factory_.get(); }
}  // namespace cru::platform::win
