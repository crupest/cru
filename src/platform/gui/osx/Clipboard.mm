#include "cru/platform/gui/osx/Clipboard.h"
#include "cru/base/platform/osx/Base.h"
#include "cru/base/log/Logger.h"

#include "ClipboardPrivate.h"

#include <memory>

namespace cru::platform::gui::osx {
OsxClipboard::OsxClipboard(cru::platform::gui::IUiApplication* ui_application,
                           std::unique_ptr<details::OsxClipboardPrivate> p)
    : OsxGuiResource(ui_application), p_(std::move(p)) {}

OsxClipboard::~OsxClipboard() {}

std::string OsxClipboard::GetText() { return p_->GetText(); }

void OsxClipboard::SetText(std::string text) { p_->SetText(text); }

namespace details {
OsxClipboardPrivate::OsxClipboardPrivate(NSPasteboard* pasteboard) : pasteboard_(pasteboard) {}

OsxClipboardPrivate::~OsxClipboardPrivate() {}

std::string OsxClipboardPrivate::GetText() {
  auto result = [pasteboard_ readObjectsForClasses:@[ NSString.class ] options:nil];
  if (result == nil) {
    CRU_LOG_TAG_WARN("Failed to get text from clipboard");
    return "";
  } else {
    if (result.count == 0) {
      return "";
    } else {
      return FromCFStringRef((CFStringRef)result[0]);
    }
  }
}

void OsxClipboardPrivate::SetText(std::string text) {
  auto cf_string = ToCFString(text);
  [pasteboard_ clearContents];
  [pasteboard_ writeObjects:@[ (NSString*)cf_string.ref ]];
}
}  // namespace details
}  // namespace cru::platform::gui::osx
