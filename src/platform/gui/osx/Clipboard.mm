#include "cru/base/log/Logger.h"
#include "cru/platform/gui/osx/Clipboard.h"

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
    CRU_LOG_WARN(u"Failed to get text from clipboard");
    return u"";
  } else {
    if (result.count == 0) {
      return u"";
    } else {
      return ::cru::String::FromCFStringRef((CFStringRef)result[0]);
    }
  }
}

void OsxClipboardPrivate::SetText(String text) {
  auto cf_string = text.ToCFStringRef();
  [pasteboard_ clearContents];
  [pasteboard_ writeObjects:@[ (NSString*)cf_string.ref ]];
}
}
}  // namespace cru::platform::gui::osx
