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
 private:
  constexpr static auto kLogTag = "cru::ui::controls::Window";
  friend Control;

 public:
  static constexpr auto kControlName = "Window";

  Window();

  static Window* CreatePopup(Control* attached_control);

  Control* GetAttachedControl();
  void SetAttachedControl(Control* control);

  platform::gui::INativeWindow* GetNativeWindow();

  void SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value);

 private:
  std::unique_ptr<ControlHost> control_host_;

  Control* attached_control_;
  EventHandlerRevokerGuard parent_window_guard_;

  EventHandlerRevokerListGuard
      gain_focus_on_create_and_destroy_when_lose_focus_event_guard_;
};
}  // namespace cru::ui::controls
