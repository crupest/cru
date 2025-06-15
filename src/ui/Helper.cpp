#include "Helper.h"

#include "cru/graphics/Factory.h"
#include "cru/gui/UiApplication.h"

namespace cru::ui {
using cru::graphics::IGraphicsFactory;
using cru::platform::gui::IUiApplication;

IGraphicsFactory* GetGraphicsFactory() {
  return IUiApplication::GetInstance()->GetGraphicsFactory();
}

IUiApplication* GetUiApplication() { return IUiApplication::GetInstance(); }
}  // namespace cru::ui
