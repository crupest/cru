// Some useful information can be found from chromium code:
// https://chromium.googlesource.com/chromium/chromium/+/refs/heads/master/ui/base/win/ime_input.h
// https://chromium.googlesource.com/chromium/chromium/+/refs/heads/master/ui/base/win/ime_input.cc

#pragma once
#include "Base.h"
#include "WindowNativeMessageEventArgs.h"

#include <cru/platform/gui/InputMethod.h>

#include <imm.h>

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API AutoHIMC {
 private:
  constexpr static auto kLogTag = "AutoHIMC";

 public:
  explicit AutoHIMC(HWND hwnd);

  CRU_DELETE_COPY(AutoHIMC)

  AutoHIMC(AutoHIMC&& other);
  AutoHIMC& operator=(AutoHIMC&& other);

  ~AutoHIMC();

  HWND GetHwnd() const { return hwnd_; }

  HIMC Get() const { return handle_; }

 private:
  HWND hwnd_;
  HIMC handle_;
};

class CRU_WIN_GUI_API WinInputMethodContext
    : public WinNativeResource,
      public virtual IInputMethodContext {
 private:
  constexpr static auto kLogTag = "WinInputMethodContext";

 public:
  WinInputMethodContext(WinNativeWindow* window);

  ~WinInputMethodContext() override;

  bool ShouldManuallyDrawCompositionText() override { return true; }

  void EnableIME() override;

  void DisableIME() override;

  void CompleteComposition() override;

  void CancelComposition() override;

  CompositionText GetCompositionText() override;

  void SetCandidateWindowPosition(const Point& point) override;

  IEvent<std::nullptr_t>* CompositionStartEvent() override;

  IEvent<std::nullptr_t>* CompositionEndEvent() override;

  IEvent<std::nullptr_t>* CompositionEvent() override;

  IEvent<const std::string&>* TextEvent() override;

 private:
  void OnWindowNativeMessage(WindowNativeMessageEventArgs& args);

  std::string GetResultString();

  AutoHIMC GetHIMC();

 private:
  WinNativeWindow* native_window_;

  EventHandlerRevokerListGuard event_guard_;

  Event<std::nullptr_t> composition_start_event_;
  Event<std::nullptr_t> composition_end_event_;
  Event<std::nullptr_t> composition_event_;
  Event<const std::string&> text_event_;
};
}  // namespace cru::platform::gui::win
