#include "cru/platform/gui/win/InputMethod.h"
#include "cru/base/StringUtil.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/DebugFlags.h"
#include "cru/platform/gui/win/Window.h"

#include <vector>

namespace cru::platform::gui::win {
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
      CRU_LOG_TAG_WARN("Failed to release HIMC.");
  }
}

// copied from chromium
namespace {
// Determines whether or not the given attribute represents a target
// (a.k.a. a selection).
bool IsTargetAttribute(char attribute) {
  return (attribute == ATTR_TARGET_CONVERTED ||
          attribute == ATTR_TARGET_NOTCONVERTED);
}
// Helper function for ImeInput::GetCompositionInfo() method, to get the target
// range that's selected by the user in the current composition string.
void GetCompositionTargetRange(HIMC imm_context, int* target_start,
                               int* target_end) {
  int attribute_size =
      ::ImmGetCompositionString(imm_context, GCS_COMPATTR, NULL, 0);
  if (attribute_size > 0) {
    int start = 0;
    int end = 0;
    std::vector<char> attribute_data(attribute_size);
    ::ImmGetCompositionString(imm_context, GCS_COMPATTR, attribute_data.data(),
                              attribute_size);
    for (start = 0; start < attribute_size; ++start) {
      if (IsTargetAttribute(attribute_data[start])) break;
    }
    for (end = start; end < attribute_size; ++end) {
      if (!IsTargetAttribute(attribute_data[end])) break;
    }
    if (start == attribute_size) {
      // This composition clause does not contain any target clauses,
      // i.e. this clauses is an input clause.
      // We treat the whole composition as a target clause.
      start = 0;
      end = attribute_size;
    }
    *target_start = start;
    *target_end = end;
  }
}
// Helper function for ImeInput::GetCompositionInfo() method, to get underlines
// information of the current composition string.
CompositionClauses GetCompositionClauses(HIMC imm_context, int target_start,
                                         int target_end) {
  CompositionClauses result;
  int clause_size =
      ::ImmGetCompositionString(imm_context, GCS_COMPCLAUSE, NULL, 0);
  int clause_length = clause_size / sizeof(std::uint32_t);
  if (clause_length) {
    result.reserve(clause_length - 1);
    std::vector<std::uint32_t> clause_data(clause_length);
    ::ImmGetCompositionString(imm_context, GCS_COMPCLAUSE, clause_data.data(),
                              clause_size);
    for (int i = 0; i < clause_length - 1; ++i) {
      CompositionClause clause;
      clause.start = clause_data[i];
      clause.end = clause_data[i + 1];
      clause.target = false;
      // Use thick underline for the target clause.
      if (clause.start >= target_start && clause.end <= target_end) {
        clause.target = true;
      }
      result.push_back(clause);
    }
  }
  return result;
}

std::wstring GetString(HIMC imm_context) {
  LONG string_size =
      ::ImmGetCompositionString(imm_context, GCS_COMPSTR, NULL, 0);
  std::wstring result((string_size / sizeof(wchar_t)), 0);
  ::ImmGetCompositionString(imm_context, GCS_COMPSTR, result.data(),
                            string_size);
  return result;
}

std::wstring GetResultString(HIMC imm_context) {
  LONG string_size =
      ::ImmGetCompositionString(imm_context, GCS_RESULTSTR, NULL, 0);
  std::wstring result((string_size / sizeof(wchar_t)), 0);
  ::ImmGetCompositionString(imm_context, GCS_RESULTSTR, result.data(),
                            string_size);
  return result;
}

CompositionText GetCompositionInfo(HIMC imm_context) {
  // We only care about GCS_COMPATTR, GCS_COMPCLAUSE and GCS_CURSORPOS, and
  // convert them into underlines and selection range respectively.

  auto utf16_text = GetString(imm_context);
  auto text = string::ToUtf8String(utf16_text);

  int length = static_cast<int>(utf16_text.length());
  // Find out the range selected by the user.
  int target_start = length;
  int target_end = length;
  GetCompositionTargetRange(imm_context, &target_start, &target_end);

  auto clauses = GetCompositionClauses(imm_context, target_start, target_end);
  for (auto& clause : clauses) {
    clause.start = string::Utf8IndexCodePointToCodeUnit(
        text, string::Utf16IndexCodeUnitToCodePoint(utf16_text, clause.start));
    clause.end = string::Utf8IndexCodePointToCodeUnit(
        text, string::Utf16IndexCodeUnitToCodePoint(utf16_text, clause.end));
  }

  int cursor = string::Utf8IndexCodePointToCodeUnit(
      text, string::Utf16IndexCodeUnitToCodePoint(
                utf16_text, ::ImmGetCompositionString(imm_context,
                                                      GCS_CURSORPOS, NULL, 0)));

  return CompositionText{std::move(text), std::move(clauses),
                         TextRange{cursor}};
}

}  // namespace

WinInputMethodContext::WinInputMethodContext(WinNativeWindow* window)
    : native_window_(window) {
  event_guard_ += window->NativeMessageEvent()->AddHandler(
      std::bind(&WinInputMethodContext::OnWindowNativeMessage, this,
                std::placeholders::_1));
}

WinInputMethodContext::~WinInputMethodContext() {}

void WinInputMethodContext::EnableIME() {
  const auto hwnd = native_window_->GetWindowHandle();
  if (::ImmAssociateContextEx(hwnd, nullptr, IACE_DEFAULT) == FALSE) {
    CRU_LOG_TAG_WARN("Failed to enable ime.");
  }
}

void WinInputMethodContext::DisableIME() {
  const auto hwnd = native_window_->GetWindowHandle();
  AutoHIMC himc{hwnd};

  ::ImmNotifyIME(himc.Get(), NI_COMPOSITIONSTR, CPS_COMPLETE, 0);

  if (::ImmAssociateContextEx(hwnd, nullptr, 0) == FALSE) {
    CRU_LOG_TAG_WARN("Failed to disable ime.");
  }
}

void WinInputMethodContext::CompleteComposition() {
  auto himc = GetHIMC();
  if (!::ImmNotifyIME(himc.Get(), NI_COMPOSITIONSTR, CPS_COMPLETE, 0)) {
    CRU_LOG_TAG_WARN("Failed to complete composition.");
  }
}

void WinInputMethodContext::CancelComposition() {
  auto himc = GetHIMC();
  if (!::ImmNotifyIME(himc.Get(), NI_COMPOSITIONSTR, CPS_CANCEL, 0)) {
    CRU_LOG_TAG_WARN("Failed to complete composition.");
  }
}

CompositionText WinInputMethodContext::GetCompositionText() {
  auto himc = GetHIMC();
  return GetCompositionInfo(himc.Get());
}

void WinInputMethodContext::SetCandidateWindowPosition(const Point& point) {
  auto himc = GetHIMC();

  ::CANDIDATEFORM form;
  form.dwIndex = 0;
  form.dwStyle = CFS_CANDIDATEPOS;

  form.ptCurrentPos = native_window_->DipToPixel(point);

  if (!::ImmSetCandidateWindow(himc.Get(), &form))
    CRU_LOG_TAG_DEBUG("Failed to set input method candidate window position.");
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

IEvent<const std::string&>* WinInputMethodContext::TextEvent() {
  return &text_event_;
}

void WinInputMethodContext::OnWindowNativeMessage(
    WindowNativeMessageEventArgs& args) {
  const auto& message = args.GetWindowMessage();
  switch (message.msg) {
    case WM_CHAR: {
      auto c = static_cast<char16_t>(message.w_param);
      if (cru::string::IsUtf16SurrogatePairCodeUnit(c)) {
        // I don't think this will happen because normal key strike without ime
        // should only trigger ascci character. If it is a charater from
        // supplementary planes, it should be handled with ime messages.
        CRU_LOG_TAG_WARN(
            "A WM_CHAR message for character from supplementary "
            "planes is ignored.");
      } else {
        if (c != '\b') {            // ignore backspace
          if (c == '\r') c = '\n';  // Change \r to \n
          text_event_.Raise(std::string{static_cast<char>(c)});
        }
      }
      args.HandleWithResult(0);
      break;
    }
    case WM_IME_COMPOSITION: {
      composition_event_.Raise(nullptr);
      auto composition_text = GetCompositionText();
      if constexpr (DebugFlags::input_method) {
        CRU_LOG_TAG_DEBUG("WM_IME_COMPOSITION composition text:\n{}",
                          composition_text);
      }
      if (message.l_param & GCS_RESULTSTR) {
        auto result_string = GetResultString();
        text_event_.Raise(result_string);
      }
      break;
    }
    case WM_IME_STARTCOMPOSITION: {
      if constexpr (DebugFlags::input_method) {
        CRU_LOG_TAG_DEBUG("WM_IME_STARTCOMPOSITION received.");
      }
      composition_start_event_.Raise(nullptr);
      break;
    }
    case WM_IME_ENDCOMPOSITION: {
      if constexpr (DebugFlags::input_method) {
        CRU_LOG_TAG_DEBUG("WM_IME_ENDCOMPOSITION received.");
      }
      composition_end_event_.Raise(nullptr);
      break;
    }
  }
}

std::string WinInputMethodContext::GetResultString() {
  auto himc = GetHIMC();
  auto result = win::GetResultString(himc.Get());
  return string::ToUtf8String(result);
}

AutoHIMC WinInputMethodContext::GetHIMC() {
  const auto hwnd = native_window_->GetWindowHandle();
  return AutoHIMC{hwnd};
}
}  // namespace cru::platform::gui::win
