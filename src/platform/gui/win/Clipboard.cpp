#include "cru/platform/gui/win/Clipboard.h"
#include "cru/base/StringUtil.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/win/UiApplication.h"

namespace cru::platform::gui::win {
WinClipboard::WinClipboard(WinUiApplication* application)
    : application_(application) {}

WinClipboard::~WinClipboard() {}

std::string WinClipboard::GetText() {
  if (!::OpenClipboard(nullptr)) {
    CRU_LOG_TAG_WARN("Failed to open clipboard.");
    return {};
  }

  if (!::IsClipboardFormatAvailable(CF_UNICODETEXT)) {
    CRU_LOG_TAG_WARN("Clipboard format for text is not available.");
    return {};
  }

  auto handle = ::GetClipboardData(CF_UNICODETEXT);

  if (handle == nullptr) {
    CRU_LOG_TAG_WARN("Failed to get clipboard data.");
    return {};
  }

  auto ptr = ::GlobalLock(handle);
  if (ptr == nullptr) {
    CRU_LOG_TAG_WARN("Failed to lock clipboard data.");
    ::CloseClipboard();
    return {};
  }

  std::wstring result(static_cast<wchar_t*>(ptr));

  ::GlobalUnlock(handle);
  ::CloseClipboard();

  return string::ToUtf8(result);
}

void WinClipboard::SetText(std::string utf8_text) {
  auto text = string::ToUtf16(utf8_text);

  if (!::OpenClipboard(nullptr)) {
    CRU_LOG_TAG_WARN("Failed to open clipboard.");
    return;
  }

  auto handle = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));

  if (handle == nullptr) {
    CRU_LOG_TAG_WARN("Failed to allocate clipboard data.");
    ::CloseClipboard();
    return;
  }

  auto ptr = ::GlobalLock(handle);
  if (ptr == nullptr) {
    CRU_LOG_TAG_WARN("Failed to lock clipboard data.");
    ::GlobalFree(handle);
    ::CloseClipboard();
    return;
  }

  std::memcpy(ptr, text.c_str(), (text.size() + 1) * sizeof(wchar_t));

  ::GlobalUnlock(handle);

  if (::SetClipboardData(CF_UNICODETEXT, handle) == nullptr) {
    CRU_LOG_TAG_WARN("Failed to set clipboard data.");
  }

  ::CloseClipboard();
}
}  // namespace cru::platform::gui::win
