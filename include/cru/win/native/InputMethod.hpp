// Some useful information can be found from chromium code:
// https://chromium.googlesource.com/chromium/chromium/+/refs/heads/master/ui/base/win/ime_input.h
// https://chromium.googlesource.com/chromium/chromium/+/refs/heads/master/ui/base/win/ime_input.cc

#pragma once
#include "Resource.hpp"

#include "WindowNativeMessageEventArgs.hpp"
#include "cru/platform/native/InputMethod.hpp"

#include <imm.h>

namespace cru::platform::native::win {
class AutoHIMC : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::native::win::AutoHIMC")

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
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::native::win::WinInputMethodContext")

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

  CompositionText GetCompositionText() override;

  void SetCandidateWindowPosition(const Point& point) override;

  IEvent<std::nullptr_t>* CompositionStartEvent() override;

  IEvent<std::nullptr_t>* CompositionEndEvent() override;

  IEvent<std::nullptr_t>* CompositionEvent() override;

  IEvent<std::string_view>* TextEvent() override;

 private:
  void OnWindowNativeMessage(WindowNativeMessageEventArgs& args);

  std::string GetResultString();

  std::optional<AutoHIMC> TryGetHIMC();

 private:
  std::shared_ptr<INativeWindowResolver> native_window_resolver_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  Event<std::nullptr_t> composition_start_event_;
  Event<std::nullptr_t> composition_end_event_;
  Event<std::nullptr_t> composition_event_;
  Event<std::string_view> text_event_;
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
