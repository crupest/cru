#pragma once
#include "cru/ui/Base.hpp"

namespace cru::platform {
namespace graph {
struct IGraphFactory;
}
namespace native {
struct ICursor;
struct IUiApplication;
}  // namespace native
}  // namespace cru::platform

namespace cru::ui {
cru::platform::graphics::IGraphFactory* GetGraphFactory();
cru::platform::gui::IUiApplication* GetUiApplication();
}  // namespace cru::ui
