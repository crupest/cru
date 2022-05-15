#include "cru/platform/gui/osx/Clipboard.h"
#include "ClipboardPrivate.h"

#include "cru/common/log/Logger.h"
#include "cru/platform/osx/Convert.h"

#include <memory>

namespace cru::platform::gui::osx {
using cru::platform::osx::Convert;

OsxClipboard::OsxClipboard(cru::platform::gui::IUiApplication* ui_application,
                           std::unique_ptr<details::OsxClipboardPrivate> p)
    : OsxGuiResource(ui_application), p_(std::move(p)) {}

OsxClipboard::~OsxClipboard() {}

String OsxClipboard::GetText() { return p_->GetText(); }

void OsxClipboard::SetText(String text) { p_->SetText(text); }

namespace details {
OsxClipboardPrivate::OsxClipboardPrivate(NSPasteboard* pasteboard) : pasteboard_(pasteboard) {}

OsxClipboardPrivate::~OsxClipboardPrivate() {}

String OsxClipboardPrivate::GetText() {
  auto result = [pasteboard_ readObjectsForClasses:@[ NSString.class ] options:nil];
  if (result == nil) {
    CRU_LOG_WARN(u"Failed to get text from clipboard");
    return u"";
  } else {
    if (result.count == 0) {
      return u"";
    } else {
      return Convert((CFStringRef)result[0]);
    }
  }
}

void OsxClipboardPrivate::SetText(String text) {
  [pasteboard_ clearContents];
  [pasteboard_ writeObjects:@[ (NSString*)Convert(text) ]];
}
}
}  // namespace cru::platform::gui::osx
