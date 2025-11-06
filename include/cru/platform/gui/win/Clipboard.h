#pragma once
#include "Base.h"

#include <cru/platform/gui/Clipboard.h>

namespace cru::platform::gui::win {
class WinClipboard : public WinNativeResource, public virtual IClipboard {
  CRU_DEFINE_CLASS_LOG_TAG("WinClipboard")
 public:
  explicit WinClipboard(WinUiApplication* application);

  ~WinClipboard() override;

 public:
  std::string GetText() override;
  void SetText(std::string text) override;

 private:
  WinUiApplication* application_;
};
}  // namespace cru::platform::gui::win
