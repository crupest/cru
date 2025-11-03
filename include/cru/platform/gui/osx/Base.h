#pragma once
#include <cru/platform/Base.h>

#include <cru/platform/gui/UiApplication.h>

namespace cru::platform::gui::osx {
class OsxGuiResource : public Object, public virtual IPlatformResource {
 public:
  explicit OsxGuiResource(IUiApplication* ui_application);
  ~OsxGuiResource() override = default;

 public:
  std::string GetPlatformId() const override { return "OSX GUI"; }

  IUiApplication* GetUiApplication() const { return ui_application_; }

 private:
  IUiApplication* ui_application_;
};
}  // namespace cru::platform::gui::osx
