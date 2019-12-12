#include "helper.hpp"

#include "cru/platform/graph/factory.hpp"
#include "cru/platform/native/ui_application.hpp"

namespace cru::ui {
using cru::platform::graph::IGraphFactory;
using cru::platform::native::IUiApplication;

IGraphFactory* GetGraphFactory() {
  return IUiApplication::GetInstance()->GetGraphFactory();
}

IUiApplication* GetUiApplication() { return IUiApplication::GetInstance(); }
}  // namespace cru::ui
