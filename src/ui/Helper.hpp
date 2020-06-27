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
cru::platform::graph::IGraphFactory* GetGraphFactory();
cru::platform::native::IUiApplication* GetUiApplication();
}  // namespace cru::ui
