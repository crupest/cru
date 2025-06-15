#pragma once
#include "cru/osx/Resource.h"

#include "cru/gui/Base.h"

namespace cru::platform::gui::osx {
class OsxGuiResource : public platform::osx::OsxResource {
 public:
  explicit OsxGuiResource(IUiApplication* ui_application);


  ~OsxGuiResource() override = default;

 public:
  String GetPlatformId() const override { return u"OSX GUI"; }

  IUiApplication* GetUiApplication() const { return ui_application_; }

 private:
  IUiApplication* ui_application_;
};
}  // namespace cru::platform::gui::osx
