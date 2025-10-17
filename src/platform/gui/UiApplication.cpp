#include "cru/platform/gui/UiApplication.h"

#include "cru/base/Exception.h"

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

std::optional<String> IUiApplication::ShowSaveDialog(
    SaveDialogOptions options) {
  throw Exception("Not implemented.");
}

std::optional<std::vector<String>> IUiApplication::ShowOpenDialog(
    OpenDialogOptions options) {
  throw Exception("Not implemented.");
}
}  // namespace cru::platform::gui
