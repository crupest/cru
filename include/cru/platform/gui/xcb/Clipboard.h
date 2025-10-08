#pragma once

#include "../Clipboard.h"
#include "Base.h"

namespace cru::platform::gui::xcb {
class XcbUiApplication;
class XcbClipboard : public XcbResource, public virtual IClipboard {
 public:
  explicit XcbClipboard(XcbUiApplication* application);
  ~XcbClipboard() override;

  String GetText() override;
  void SetText(String text) override;

 private:
  XcbUiApplication* application_;
};
}  // namespace cru::platform::gui::xcb
