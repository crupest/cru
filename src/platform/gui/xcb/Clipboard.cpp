#include "cru/platform/gui/xcb/Clipboard.h"
#include "cru/platform/gui/xcb/UiApplication.h"

namespace cru::platform::gui::xcb {
XcbClipboard::XcbClipboard(XcbUiApplication* application)
    : application_(application) {}

XcbClipboard::~XcbClipboard() {}

String XcbClipboard::GetText() { return String{}; }

void XcbClipboard::SetText(String text) {}
}  // namespace cru::platform::gui::xcb
