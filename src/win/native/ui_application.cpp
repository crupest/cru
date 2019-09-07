#include "cru/win/native/ui_application.hpp"

#include "cru/win/graph/direct/graph_factory.hpp"
#include "cru/win/native/exception.hpp"
#include "cru/win/native/god_window.hpp"
#include "cru/win/native/native_window.hpp"
#include "god_window_message.hpp"
#include "timer.hpp"
#include "window_manager.hpp"

#include <VersionHelpers.h>
#include <cassert>

namespace cru::platform::native::win {
namespace {
WinUiApplication* instance = nullptr;
}
}  // namespace cru::platform::native::win

namespace cru::platform::native {
UiApplication* UiApplication::CreateInstance() {
  auto& i =
      ::cru::platform::native::win::instance;  // avoid long namespace prefix
  assert(i == nullptr);
  i = new win::WinUiApplication(::GetModuleHandleW(nullptr));
  return i;
}

UiApplication* UiApplication::GetInstance() {
  return ::cru::platform::native::win::instance;
}
}  // namespace cru::platform::native

namespace cru::platform::native::win {
WinUiApplication* WinUiApplication::GetInstance() { return instance; }

WinUiApplication::WinUiApplication(HINSTANCE h_instance)
    : h_instance_(h_instance) {
  assert(instance == nullptr);

  if (!::IsWindows8OrGreater())
    throw std::runtime_error("Must run on Windows 8 or later.");

  const auto graph_factory = graph::GraphFactory::CreateInstance();
  graph_factory->SetAutoDelete(true);

  god_window_ = std::make_shared<GodWindow>(this);
  timer_manager_ = std::make_shared<TimerManager>(god_window_.get());
  window_manager_ = std::make_shared<WindowManager>(this);

  cursor_manager_.reset(new WinCursorManager());
}

WinUiApplication::~WinUiApplication() { instance = nullptr; }

int WinUiApplication::Run() {
  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  for (const auto& handler : quit_handlers_) handler();

  if (auto_delete_) delete this;

  return static_cast<int>(msg.wParam);
}

void WinUiApplication::Quit(const int quit_code) {
  ::PostQuitMessage(quit_code);
}

void WinUiApplication::AddOnQuitHandler(const std::function<void()>& handler) {
  quit_handlers_.push_back(handler);
}

void WinUiApplication::InvokeLater(const std::function<void()>& action) {
  // copy the action to a safe place
  auto p_action_copy = new std::function<void()>(action);

  if (PostMessageW(GetGodWindow()->GetHandle(), invoke_later_message_id,
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

std::vector<NativeWindow*> WinUiApplication::GetAllWindow() {
  const auto&& windows = window_manager_->GetAllWindows();
  std::vector<NativeWindow*> result;
  for (const auto w : windows) {
    result.push_back(static_cast<NativeWindow*>(w));
  }
  return result;
}

NativeWindow* WinUiApplication::CreateWindow(NativeWindow* parent) {
  WinNativeWindow* p = nullptr;
  if (parent != nullptr) {
    p = dynamic_cast<WinNativeWindow*>(parent);
    assert(p);
  }
  return new WinNativeWindow(this, window_manager_->GetGeneralWindowClass(),
                             WS_OVERLAPPEDWINDOW, p);
}

WinCursorManager* WinUiApplication::GetCursorManager() {
  return cursor_manager_.get();
}
}  // namespace cru::platform::native::win
