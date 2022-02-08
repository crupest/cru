#include "Helper.h"

#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui {
using cru::platform::graphics::IGraphicsFactory;
using cru::platform::gui::IUiApplication;

IGraphicsFactory* GetGraphicsFactory() {
  return IUiApplication::GetInstance()->GetGraphicsFactory();
}

IUiApplication* GetUiApplication() { return IUiApplication::GetInstance(); }
}  // namespace cru::ui
