#include "Helper.hpp"

#include "cru/platform/graph/Factory.hpp"
#include "cru/platform/native/UiApplication.hpp"

namespace cru::ui {
using cru::platform::graph::IGraphFactory;
using cru::platform::native::IUiApplication;

IGraphFactory* GetGraphFactory() {
  return IUiApplication::GetInstance()->GetGraphFactory();
}

IUiApplication* GetUiApplication() { return IUiApplication::GetInstance(); }
}  // namespace cru::ui
