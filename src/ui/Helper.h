#pragma once
#include "cru/ui/Base.h"

namespace cru::platform {
namespace graphics {
struct IGraphicsFactory;
}
namespace native {
struct IUiApplication;
}  // namespace native
}  // namespace cru::platform

namespace cru::ui {
cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory();
cru::platform::gui::IUiApplication* GetUiApplication();
}  // namespace cru::ui
