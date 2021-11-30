#pragma once
#include "LayoutControl.hpp"

#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Window.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/host/WindowHost.hpp"

namespace cru::ui::controls {
class RootControl : public LayoutControl {
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
