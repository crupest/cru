#pragma once
#include "Base.h"

#include <cru/platform/gui/Clipboard.h>

namespace cru::platform::gui::win {
class WinClipboard : public WinNativeResource, public virtual IClipboard {
 private:
  constexpr static auto kLogTag = "WinClipboard";

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
