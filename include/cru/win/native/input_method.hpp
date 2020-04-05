// Some useful information can be found from chromium code:
// https://chromium.googlesource.com/chromium/chromium/+/refs/heads/master/ui/base/win/ime_input.h

#pragma once
#include "resource.hpp"

#include "cru/platform/native/input_method.hpp"
#include "window_native_message_event_args.hpp"

#include <imm.h>

namespace cru::platform::native::win {
class WinNativeWindow;

class WinInputMethodContextRef : public WinNativeResource,
                                 public IInputMethodContextRef {
 public:
  WinInputMethodContextRef(WinNativeWindow* window);

  CRU_DELETE_COPY(WinInputMethodContextRef)
  CRU_DELETE_MOVE(WinInputMethodContextRef)

  ~WinInputMethodContextRef() override;

  ::HIMC GetHandle() const { return handle_; }

  bool ShouldManuallyDrawCompositionText() override { return true; }

  void Reset() override;

  std::string GetCompositionText() override;

  void SetCandidateWindowPosition(const Point& point) override;

  IEvent<std::nullptr_t>* CompositionStartEvent() override;

  IEvent<std::nullptr_t>* CompositionEndEvent() override;

  IEvent<std::string>* CompositionTextChangeEvent() override;

 private:
  void OnWindowNativeMessage(WindowNativeMessageEventArgs& args);

 private:
  [[maybe_unused]] WinNativeWindow* window_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  ::HWND window_handle_;
  ::HIMC handle_;

  Event<std::nullptr_t> composition_start_event_;
  Event<std::nullptr_t> composition_end_event_;
  Event<std::string> composition_text_change_event_;
};
}  // namespace cru::platform::native::win
