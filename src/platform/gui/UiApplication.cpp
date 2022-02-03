#include "cru/platform/gui/UiApplication.hpp"

namespace cru::platform::gui {
IUiApplication* IUiApplication::instance = nullptr;

IUiApplication::IUiApplication() {
  if (instance) {
    throw std::runtime_error("An ui application has already been created.");
  }

  instance = this;
}

IUiApplication::~IUiApplication() { instance = nullptr; }

IMenu* IUiApplication::GetApplicationMenu() { return nullptr; }
}  // namespace cru::platform::gui
