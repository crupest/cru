#pragma once
#include "cru/platform/osx/Resource.h"

#include <cru/platform/gui/UiApplication.h>

namespace cru::platform::gui::osx {
class OsxGuiResource : public platform::osx::OsxResource {
 public:
  explicit OsxGuiResource(IUiApplication* ui_application);

  CRU_DELETE_COPY(OsxGuiResource)
  CRU_DELETE_MOVE(OsxGuiResource)

  ~OsxGuiResource() override = default;

 public:
  std::string GetPlatformId() const override { return "OSX GUI"; }

  IUiApplication* GetUiApplication() const { return ui_application_; }

 private:
  IUiApplication* ui_application_;
};
}  // namespace cru::platform::gui::osx
