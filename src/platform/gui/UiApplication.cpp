#include "cru/platform/gui/UiApplication.h"

#include "cru/base/Base.h"

namespace cru::platform::gui {

namespace {
IUiApplication* instance = nullptr;
}

IUiApplication* IUiApplication::GetInstance() { return instance; }

IUiApplication::IUiApplication() {
  if (instance) {
    throw std::runtime_error("An ui application has already been created.");
  }

  instance = this;
}

IUiApplication::~IUiApplication() { instance = nullptr; }

IMenu* IUiApplication::GetApplicationMenu() { return nullptr; }

std::optional<std::string> IUiApplication::ShowSaveDialog(
    SaveDialogOptions options) {
  NotImplemented();
}

std::optional<std::vector<std::string>> IUiApplication::ShowOpenDialog(
    OpenDialogOptions options) {
  NotImplemented();
}
}  // namespace cru::platform::gui
