#pragma once
#include "LayoutControl.hpp"

#include "cru/common/Base.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/host/WindowHost.hpp"

namespace cru::ui::controls {
class RootControl : public LayoutControl {
 protected:
  explicit RootControl(Control* attached_control,
                       host::CreateWindowParams params);

 public:
  CRU_DELETE_COPY(RootControl)
  CRU_DELETE_MOVE(RootControl)
  ~RootControl() override;

 public:
  render::RenderObject* GetRenderObject() const override;

 protected:
  void SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value);

 private:
  platform::gui::INativeWindow* GetNativeWindow(bool create);

 private:
  std::unique_ptr<host::WindowHost> window_host_;

  std::unique_ptr<render::StackLayoutRenderObject> render_object_;

  Control* attached_control_;

  bool gain_focus_on_create_and_destroy_when_lose_focus_ = false;
};
}  // namespace cru::ui::controls
