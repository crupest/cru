#include "cru/platform/win/win_font.hpp"

#include "cru/platform/win/exception.hpp"
#include "cru/platform/win/graph_manager.hpp"

#include <array>
#include <utility>

namespace cru::platform::win {
WinFontDescriptor::WinFontDescriptor(GraphManager* graph_manager,
                                     const std::wstring_view& font_family,
                                     float font_size) {
  std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> buffer;
  if (!::GetUserDefaultLocaleName(buffer.data(), buffer.size()))
    throw Win32Error(::GetLastError(), "Failed to get locale.");

  ThrowIfFailed(graph_manager->GetDWriteFactory()->CreateTextFormat(
      font_family.data(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, font_size,
      buffer.data(), &text_format_));
}
}  // namespace cru::platform::win
