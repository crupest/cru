#pragma once
#include "Resource.h"

#include "cru/gui/Base.h"
#include "cru/gui/Clipboard.h"

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
  String GetText() override;
  void SetText(String text) override;

 private:
  std::unique_ptr<details::OsxClipboardPrivate> p_;
};
}  // namespace cru::platform::gui::osx
