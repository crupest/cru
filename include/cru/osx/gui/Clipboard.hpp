#pragma once
#include "Resource.hpp"

#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Clipboard.hpp"

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
  String GetText() override;
  void SetText(String text) override;

 private:
  std::unique_ptr<details::OsxClipboardPrivate> p_;
};
}  // namespace cru::platform::gui::osx
