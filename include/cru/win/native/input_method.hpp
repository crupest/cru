#pragma once
#include "resource.hpp"

#include "cru/platform/native/input_method.hpp"

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

  void Reset() override;

  std::string GetCompositionString() override;

  void SetCandidateWindowPosition(const Point& point) override;

  IEvent<std::string>* CompositionTextChangeEvent() override;

  IEvent<std::string>* CharEvent() override;

 private:
  WinNativeWindow* window_;

  ::HWND window_handle_;
  ::HIMC handle_;

  Event<std::string> composition_text_change_event_;
  Event<std::string> char_event_;
};
}  // namespace cru::platform::native::win
