#pragma once
#include "Resource.h"
#include "cru/common/Base.h"
#include "cru/platform/gui/Clipboard.h"
#include "cru/win/gui/Base.h"

namespace cru::platform::gui::win {
class WinClipboard : public WinNativeResource, public virtual IClipboard {
 public:
  explicit WinClipboard(WinUiApplication* application);

  CRU_DELETE_COPY(WinClipboard)
  CRU_DELETE_MOVE(WinClipboard)

  ~WinClipboard() override;

 public:
  String GetText() override;
  void SetText(String text) override;

 private:
  WinUiApplication* application_;
};
}  // namespace cru::platform::gui::win
