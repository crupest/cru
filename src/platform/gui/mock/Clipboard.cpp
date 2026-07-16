#include "cru/platform/gui/mock/Clipboard.h"

#include <utility>

namespace cru::platform::gui::mock {
std::string MockClipboard::GetText() { return text_; }

void MockClipboard::SetText(std::string text) { text_ = std::move(text); }

void MockClipboard::Clear() { text_.clear(); }
}  // namespace cru::platform::gui::mock
