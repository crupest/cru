// Some useful information can be found from chromium code:
// https://chromium.googlesource.com/chromium/chromium/+/refs/heads/master/ui/base/win/ime_input.h
// https://chromium.googlesource.com/chromium/chromium/+/refs/heads/master/ui/base/win/ime_input.cc

#pragma once
#include "resource.hpp"

#include "cru/platform/native/input_method.hpp"
#include "window_native_message_event_args.hpp"

#include <imm.h>

namespace cru::platform::native::win {
class AutoHIMC : public Object {
 public:
  explicit AutoHIMC(HWND hwnd);

  CRU_DELETE_COPY(AutoHIMC)

  AutoHIMC(AutoHIMC&& other);
  AutoHIMC& operator=(AutoHIMC&& other);

  ~AutoHIMC() override;

  HWND GetHwnd() const { return hwnd_; }

  HIMC Get() const { return handle_; }

 private:
  HWND hwnd_;
  HIMC handle_;
};

class WinInputMethodContext : public WinNativeResource,
                              public virtual IInputMethodContext {
 public:
  WinInputMethodContext(gsl::not_null<WinNativeWindow*> window);

  CRU_DELETE_COPY(WinInputMethodContext)
  CRU_DELETE_MOVE(WinInputMethodContext)

  ~WinInputMethodContext() override;

  bool ShouldManuallyDrawCompositionText() override { return true; }

  void EnableIME() override;

  void DisableIME() override;

  void CompleteComposition() override;

  void CancelComposition() override;

  const CompositionText& GetCompositionText() override;

  void SetCandidateWindowPosition(const Point& point) override;

  IEvent<std::nullptr_t>* CompositionStartEvent() override;

  IEvent<std::nullptr_t>* CompositionEndEvent() override;

  IEvent<std::nullptr_t>* CompositionEvent() override;

 private:
  void OnWindowNativeMessage(WindowNativeMessageEventArgs& args);

  std::optional<AutoHIMC> TryGetHIMC();

 private:
  std::shared_ptr<INativeWindowResolver> native_window_resolver_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  Event<std::nullptr_t> composition_start_event_;
  Event<std::nullptr_t> composition_end_event_;
  Event<std::nullptr_t> composition_event_;
};

class WinInputMethodManager : public WinNativeResource,
                              public virtual IInputMethodManager {
 public:
  WinInputMethodManager(WinUiApplication* application);

  CRU_DELETE_COPY(WinInputMethodManager)
  CRU_DELETE_MOVE(WinInputMethodManager)

  ~WinInputMethodManager() override;

 public:
  std::unique_ptr<IInputMethodContext> GetContext(
      INativeWindow* window) override;
};
}  // namespace cru::platform::native::win
