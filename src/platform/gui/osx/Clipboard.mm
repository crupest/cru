#include "cru/platform/gui/osx/Clipboard.h"
#include "cru/base/Osx.h"
#include "cru/base/log/Logger.h"

#include "ClipboardPrivate.h"

#include <memory>

namespace cru::platform::gui::osx {
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
    CRU_LOG_TAG_WARN("Failed to get text from clipboard");
    return u"";
  } else {
    if (result.count == 0) {
      return u"";
    } else {
      return FromCFStringRef((CFStringRef)result[0]);
    }
  }
}

void OsxClipboardPrivate::SetText(String text) {
  auto cf_string = ToCFString(text);
  [pasteboard_ clearContents];
  [pasteboard_ writeObjects:@[ (NSString*)cf_string.ref ]];
}
}  // namespace details
}  // namespace cru::platform::gui::osx
