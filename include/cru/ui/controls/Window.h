#pragma once
#include "../render/StackLayoutRenderObject.h"
#include "ControlHost.h"
#include "LayoutControl.h"

#include <cru/base/Base.h>
#include <cru/base/Event.h>
#include <cru/base/Guard.h>
#include <cru/base/log/Logger.h>
#include <cru/platform/gui/UiApplication.h>
#include <cru/platform/gui/Window.h>

#include <memory>

namespace cru::ui::controls {
class CRU_UI_API Window
    : public LayoutControl<render::StackLayoutRenderObject> {
  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::controls::Window")
  friend Control;

 public:
  static constexpr std::string_view kControlType = "Window";

  Window();

  static Window* CreatePopup();

  std::string GetControlType() const override;

  void SetAttachedControl(Control* control);

  platform::gui::INativeWindow* GetNativeWindow();

  void SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value);

 private:
  std::unique_ptr<ControlHost> control_host_;

  Control* attached_control_;

  EventHandlerRevokerListGuard
      gain_focus_on_create_and_destroy_when_lose_focus_event_guard_;
};
}  // namespace cru::ui::controls
