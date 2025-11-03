#pragma once
#include "cru/base/Base.h"
#include "cru/platform/gui/osx/Clipboard.h"

#include <AppKit/AppKit.h>

namespace cru::platform::gui::osx {
namespace details {
class OsxClipboardPrivate : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("OsxClipboardPrivate")
 public:
  explicit OsxClipboardPrivate(NSPasteboard* pasteboard);
  ~OsxClipboardPrivate();

 public:
  std::string GetText();
  void SetText(std::string text);

 private:
  NSPasteboard* pasteboard_;
};
}  // namespace details
}  // namespace cru::platform::gui::osx
