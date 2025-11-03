#pragma once
#include "Base.h"

#include <cru/platform/gui/Clipboard.h>

#include <memory>

namespace cru::platform::gui::osx {
namespace details {
class OsxClipboardPrivate;
}

class OsxClipboard : public OsxGuiResource, public virtual IClipboard {
 public:
  OsxClipboard(cru::platform::gui::IUiApplication* ui_application,
               std::unique_ptr<details::OsxClipboardPrivate> p);
  ~OsxClipboard() override;

 public:
  std::string GetText() override;
  void SetText(std::string text) override;

 private:
  std::unique_ptr<details::OsxClipboardPrivate> p_;
};
}  // namespace cru::platform::gui::osx
