#include "cru/platform/gui/UiApplication.h"
#include "cru/base/Base.h"

#include <unordered_set>

namespace cru::platform::gui {

void DeleteLaterPool::Add(Object* object) { pool_.push_back(object); }

void DeleteLaterPool::Clean() {
  // Destructors of objects might add more objects to delete later. So the safe
  // implementation is to copy current pool to avoid modification during
  // iteration.
  while (!pool_.empty()) {
    std::vector<Object*> copy = std::move(pool_);
    std::unordered_set<Object*> deleted;
    for (auto object : copy) {
      if (!deleted.contains(object)) {
        deleted.insert(object);
        delete object;
      }
    }
  }
}

namespace {
IUiApplication* instance = nullptr;
}

IUiApplication* IUiApplication::GetInstance() { return instance; }

IUiApplication::IUiApplication() {
  if (instance) {
    throw Exception("A ui application has already been created.");
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
