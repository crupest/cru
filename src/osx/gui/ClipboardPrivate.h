#pragma once
#include "cru/common/Base.h"
#include "cru/osx/gui/Clipboard.h"

#include <AppKit/AppKit.h>

namespace cru::platform::gui::osx {
namespace details {
class OsxClipboardPrivate : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"OsxClipboardPrivate")
 public:
  explicit OsxClipboardPrivate(NSPasteboard* pasteboard);

  CRU_DELETE_COPY(OsxClipboardPrivate)
  CRU_DELETE_MOVE(OsxClipboardPrivate)

  ~OsxClipboardPrivate();

 public:
  String GetText();
  void SetText(String text);

 private:
  NSPasteboard* pasteboard_;
};
}  // namespace details
}  // namespace cru::platform::gui::osx
