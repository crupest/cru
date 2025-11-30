#pragma once
#include "Base.h"

#include <cru/platform/gui/InputMethod.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

namespace cru::platform::gui::sdl {
class SdlWindow;

class SdlInputMethodContext : public SdlResource,
                              public virtual IInputMethodContext {
  friend SdlWindow;

 public:
  explicit SdlInputMethodContext(SdlWindow* window);

  bool ShouldManuallyDrawCompositionText() override;
  void EnableIME() override;
  void DisableIME() override;

  /**
   * Note: SDL doesn't support this. So it actually calls CancelComposition().
   */
  void CompleteComposition() override;
  void CancelComposition() override;

  CompositionText GetCompositionText() override;

  void SetCandidateWindowPosition(const Point& point) override;

  CRU_DEFINE_CRU_PLATFORM_GUI_I_INPUT_METHOD_OVERRIDE_EVENTS()

 private:
  bool HandleEvent(const SDL_Event* event);

 private:
  SdlWindow* window_;
};
}  // namespace cru::platform::gui::sdl
