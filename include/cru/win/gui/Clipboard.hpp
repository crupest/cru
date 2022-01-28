#pragma once
#include "Resource.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/gui/Clipboard.hpp"
#include "cru/win/gui/Base.hpp"

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
