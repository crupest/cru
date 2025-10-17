#pragma once
#include <cru/platform/gui/UiApplication.h>

namespace cru::ui {
cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory();
cru::platform::gui::IUiApplication* GetUiApplication();
}  // namespace cru::ui
