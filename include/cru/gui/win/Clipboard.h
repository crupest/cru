#pragma once
#include "Resource.h"
#include <cru/Base.h>
#include "cru/gui/Clipboard.h"
#include "cru/gui/win/Base.h"

namespace cru::platform::gui::win {
class WinClipboard : public WinNativeResource, public virtual IClipboard {
  CRU_DEFINE_CLASS_LOG_TAG(u"WinClipboard")
 public:
  explicit WinClipboard(WinUiApplication* application);


  ~WinClipboard() override;

 public:
  String GetText() override;
  void SetText(String text) override;

 private:
  WinUiApplication* application_;
};
}  // namespace cru::platform::gui::win
