#pragma once
#include <cru/Base.h>
#include "cru/gui/osx/Clipboard.h"

#include <AppKit/AppKit.h>

namespace cru::platform::gui::osx {
namespace details {
class OsxClipboardPrivate : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"OsxClipboardPrivate")
 public:
  explicit OsxClipboardPrivate(NSPasteboard* pasteboard);


  ~OsxClipboardPrivate();

 public:
  String GetText();
  void SetText(String text);

 private:
  NSPasteboard* pasteboard_;
};
}  // namespace details
}  // namespace cru::platform::gui::osx
