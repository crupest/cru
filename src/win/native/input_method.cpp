#include "cru/win/native/input_method.hpp"

#include "cru/common/logger.hpp"
#include "cru/platform/check.hpp"
#include "cru/win/exception.hpp"
#include "cru/win/native/window.hpp"
#include "cru/win/string.hpp"
#include "dpi_util.hpp"

#include <vector>

namespace cru::platform::native::win {
AutoHIMC::AutoHIMC(HWND hwnd) : hwnd_(hwnd) {
  Expects(hwnd);
  handle_ = ::ImmGetContext(hwnd);
}

AutoHIMC::AutoHIMC(AutoHIMC&& other)
    : hwnd_(other.hwnd_), handle_(other.handle_) {
  other.hwnd_ = nullptr;
  other.handle_ = nullptr;
}

AutoHIMC& AutoHIMC::operator=(AutoHIMC&& other) {
  if (this != &other) {
    Object::operator=(std::move(other));
    this->hwnd_ = other.hwnd_;
    this->handle_ = other.handle_;
    other.hwnd_ = nullptr;
    other.handle_ = nullptr;
  }
  return *this;
}

AutoHIMC::~AutoHIMC() {
  if (handle_) {
    if (!::ImmReleaseContext(hwnd_, handle_))
      log::Warn("AutoHIMC: Failed to release HIMC.");
  }
}

WinInputMethodContext::WinInputMethodContext(
    gsl::not_null<WinNativeWindow*> window)
    : native_window_resolver_(window->GetResolver()) {
  event_revoker_guards_.push_back(
      EventRevokerGuard(window->NativeMessageEvent()->AddHandler(
          std::bind(&WinInputMethodContext::OnWindowNativeMessage, this,
                    std::placeholders::_1))));
}

WinInputMethodContext::~WinInputMethodContext() {}

void WinInputMethodContext::EnableIME() {
  // TODO!
}

void WinInputMethodContext::DisableIME() {
  // TODO!
}

void WinInputMethodContext::CompleteComposition() {
  // TODO!
}

void WinInputMethodContext::CancelComposition() {
  // TODO!
}

const CompositionText& WinInputMethodContext::GetCompositionText() {
  // TODO!
}

void WinInputMethodContext::SetCandidateWindowPosition(const Point& point) {
  auto optional_himc = TryGetHIMC();
  if (!optional_himc.has_value()) return;
  auto himc = std::move(optional_himc).value();

  ::CANDIDATEFORM form;
  form.dwIndex = 1;
  form.dwStyle = CFS_CANDIDATEPOS;
  form.ptCurrentPos = DipToPi(point);

  if (!::ImmSetCandidateWindow(himc.Get(), &form))
    log::Debug(
        "WinInputMethodContext: Failed to set input method candidate window "
        "position.");
}

IEvent<std::nullptr_t>* WinInputMethodContext::CompositionStartEvent() {
  return &composition_start_event_;
}

IEvent<std::nullptr_t>* WinInputMethodContext::CompositionEndEvent() {
  return &composition_end_event_;
};

IEvent<std::nullptr_t>* WinInputMethodContext::CompositionEvent() {
  return &composition_event_;
}

void WinInputMethodContext::OnWindowNativeMessage(
    WindowNativeMessageEventArgs& args) {
  const auto message = args.GetWindowMessage();
  switch (message.msg) {
    case WM_IME_COMPOSITION: {
      composition_event_.Raise(nullptr);
      break;
    }
    case WM_IME_STARTCOMPOSITION: {
      composition_start_event_.Raise(nullptr);
      break;
    }
    case WM_IME_ENDCOMPOSITION: {
      composition_end_event_.Raise(nullptr);
      break;
    }
  }
}

std::optional<AutoHIMC> WinInputMethodContext::TryGetHIMC() {
  const auto native_window = Resolve(native_window_resolver_.get());
  if (native_window == nullptr) return std::nullopt;
  const auto hwnd = native_window->GetWindowHandle();
  return AutoHIMC{hwnd};
}

WinInputMethodManager::WinInputMethodManager(WinUiApplication*) {}

WinInputMethodManager::~WinInputMethodManager() {}

std::unique_ptr<IInputMethodContext> WinInputMethodManager::GetContext(
    INativeWindow* window) {
  Expects(window);
  const auto w = CheckPlatform<WinNativeWindow>(window, GetPlatformId());
  return std::make_unique<WinInputMethodContext>(w);
}
}  // namespace cru::platform::native::win
