#include "cru/platform/gui/xcb/Clipboard.h"
#include "cru/platform/gui/xcb/UiApplication.h"

namespace cru::platform::gui::xcb {
XcbClipboard::XcbClipboard(XcbUiApplication* application)
    : application_(application) {}

XcbClipboard::~XcbClipboard() {}

std::string XcbClipboard::GetText() { return {}; }

void XcbClipboard::SetText(std::string text) {}
}  // namespace cru::platform::gui::xcb
