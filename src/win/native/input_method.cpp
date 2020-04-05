#include "cru/win/native/input_method.hpp"

#include "cru/common/logger.hpp"
#include "cru/win/exception.hpp"
#include "cru/win/native/window.hpp"
#include "cru/win/string.hpp"
#include "dpi_util.hpp"

#include <vector>

namespace cru::platform::native::win {
WinInputMethodContextRef::WinInputMethodContextRef(WinNativeWindow* window)
    : window_(window) {
  Expects(window);

  window_handle_ = window->GetWindowHandle();
  handle_ = ::ImmGetContext(window_handle_);

  // TODO: Events

  event_revoker_guards_.push_back(
      EventRevokerGuard(window->NativeMessageEvent()->AddHandler(
          std::bind(&WinInputMethodContextRef::OnWindowNativeMessage, this,
                    std::placeholders::_1))));
}

WinInputMethodContextRef::~WinInputMethodContextRef() {
  ::ImmReleaseContext(window_handle_, handle_);
}

void WinInputMethodContextRef::Reset() {
  wchar_t s[1] = {L'\0'};

  if (!::ImmSetCompositionStringW(handle_, SCS_SETSTR, static_cast<LPVOID>(s),
                                  sizeof(s), static_cast<LPVOID>(s),
                                  sizeof(s))) {
    log::Warn(
        "WinInputMethodContextRef: Failed to reset input method context.");
  }
}

std::string WinInputMethodContextRef::GetCompositionText() {
  const auto length =
      ::ImmGetCompositionStringW(handle_, GCS_RESULTREADSTR, NULL, 0) +
      sizeof(wchar_t);
  std::vector<std::byte> data(length);
  const auto result = ::ImmGetCompositionStringW(
      handle_, GCS_RESULTREADSTR, static_cast<LPVOID>(data.data()), length);

  if (result == IMM_ERROR_NODATA) {
    return std::string{};
  } else if (result == IMM_ERROR_GENERAL) {
    throw new platform::win::Win32Error(::GetLastError(),
                                        "Failed to get composition string.");
  }

  return platform::win::ToUtf8String(
      std::wstring_view(reinterpret_cast<wchar_t*>(data.data())));
}

void WinInputMethodContextRef::SetCandidateWindowPosition(const Point& point) {
  ::CANDIDATEFORM form;
  form.dwIndex = 1;
  form.dwStyle = CFS_CANDIDATEPOS;
  form.ptCurrentPos = DipToPi(point);

  if (!::ImmSetCandidateWindow(handle_, &form))
    log::Debug(
        "WinInputMethodContextRef: Failed to set input method candidate window "
        "position.");
}

IEvent<std::nullptr_t>* WinInputMethodContextRef::CompositionStartEvent() {
  return &composition_start_event_;
}

IEvent<std::nullptr_t>* WinInputMethodContextRef::CompositionEndEvent() {
  return &composition_end_event_;
};

IEvent<std::string>* WinInputMethodContextRef::CompositionTextChangeEvent() {
  return &composition_text_change_event_;
}

void WinInputMethodContextRef::OnWindowNativeMessage(
    WindowNativeMessageEventArgs& args) {
  const auto message = args.GetWindowMessage();
  switch (message.msg) {
    case WM_IME_COMPOSITION: {
      composition_text_change_event_.Raise(this->GetCompositionText());
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
}  // namespace cru::platform::native::win
