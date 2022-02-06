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

std::optional<String> IUiApplication::ShowSaveDialog(
    SaveDialogOptions options) {
  throw Exception(u"Not implemented.");
}

std::optional<std::vector<String>> IUiApplication::ShowOpenDialog(
    OpenDialogOptions options) {
  throw Exception(u"Not implemented.");
}
}  // namespace cru::platform::gui
