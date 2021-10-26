#include "Helper.hpp"

#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/gui/UiApplication.hpp"

namespace cru::ui {
using cru::platform::graphics::IGraphicsFactory;
using cru::platform::gui::IUiApplication;

IGraphicsFactory* GetGraphicsFactory() {
  return IUiApplication::GetInstance()->GetGraphicsFactory();
}

IUiApplication* GetUiApplication() { return IUiApplication::GetInstance(); }
}  // namespace cru::ui
