#pragma once

#include "../Clipboard.h"
#include "Base.h"

namespace cru::platform::gui::xcb {
class XcbUiApplication;
class XcbClipboard : public XcbResource, public virtual IClipboard {
 public:
  explicit XcbClipboard(XcbUiApplication* application);
  ~XcbClipboard() override;

  std::string GetText() override;
  void SetText(std::string text) override;

 private:
  XcbUiApplication* application_;
};
}  // namespace cru::platform::gui::xcb
