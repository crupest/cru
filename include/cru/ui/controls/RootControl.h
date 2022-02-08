#pragma once
#include "LayoutControl.h"

#include "cru/common/Base.h"
#include "cru/common/Event.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"
#include "cru/ui/host/WindowHost.h"

namespace cru::ui::controls {
class CRU_UI_API RootControl : public LayoutControl {
 protected:
  explicit RootControl(Control* attached_control);

 public:
  CRU_DELETE_COPY(RootControl)
  CRU_DELETE_MOVE(RootControl)
  ~RootControl() override;

 public:
  render::RenderObject* GetRenderObject() const override;

  platform::gui::INativeWindow* GetNativeWindow();

 protected:
  void SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value);

 private:
  std::unique_ptr<host::WindowHost> window_host_;

  std::unique_ptr<render::StackLayoutRenderObject> render_object_;

  Control* attached_control_;

  EventRevokerListGuard
      gain_focus_on_create_and_destroy_when_lose_focus_event_guard_;
};
}  // namespace cru::ui::controls