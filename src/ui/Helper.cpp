#include "Helper.hpp"

#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/gui/UiApplication.hpp"

namespace cru::ui {
using cru::platform::graphics::IGraphFactory;
using cru::platform::gui::IUiApplication;

IGraphFactory* GetGraphFactory() {
  return IUiApplication::GetInstance()->GetGraphFactory();
}

IUiApplication* GetUiApplication() { return IUiApplication::GetInstance(); }
}  // namespace cru::ui
