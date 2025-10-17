#pragma once
#include "Resource.h"

#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/Clipboard.h"

#include <memory>

namespace cru::platform::gui::osx {
namespace details {
class OsxClipboardPrivate;
}

class OsxClipboard : public OsxGuiResource, public virtual IClipboard {
 public:
  OsxClipboard(cru::platform::gui::IUiApplication* ui_application,
               std::unique_ptr<details::OsxClipboardPrivate> p);

  CRU_DELETE_COPY(OsxClipboard)
  CRU_DELETE_MOVE(OsxClipboard)

  ~OsxClipboard() override;

 public:
  std::string GetText() override;
  void SetText(std::string text) override;

 private:
  std::unique_ptr<details::OsxClipboardPrivate> p_;
};
}  // namespace cru::platform::gui::osx
