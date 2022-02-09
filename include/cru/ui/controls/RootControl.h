#pragma once
#include "LayoutControl.h"

#include "cru/common/Event.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/host/WindowHost.h"
#include "cru/ui/render/StackLayoutRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API RootControl
    : public LayoutControl<render::StackLayoutRenderObject> {
 protected:
  explicit RootControl(Control* attached_control);

 public:
  CRU_DELETE_COPY(RootControl)
  CRU_DELETE_MOVE(RootControl)
  ~RootControl() override;

 public:
  host::WindowHost* GetWindowHost() const override {
    return window_host_.get();
  }

  platform::gui::INativeWindow* GetNativeWindow();

 protected:
  void SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value);

 private:
  std::unique_ptr<host::WindowHost> window_host_;

  Control* attached_control_;

  EventRevokerListGuard
      gain_focus_on_create_and_destroy_when_lose_focus_event_guard_;
};
}  // namespace cru::ui::controls
