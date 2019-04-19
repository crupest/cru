#include "cru/win/graph/win_font.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/graph_manager.hpp"

#include <array>
#include <cassert>
#include <utility>

namespace cru::win::graph {
WinFontDescriptor::WinFontDescriptor(GraphManager* graph_manager,
                                     const std::wstring_view& font_family,
                                     float font_size) {
  assert(graph_manager);
  std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> buffer;
  if (!::GetUserDefaultLocaleName(buffer.data(), static_cast<int>(buffer.size())))
    throw Win32Error(::GetLastError(), "Failed to get locale.");

  ThrowIfFailed(graph_manager->GetDWriteFactory()->CreateTextFormat(
      font_family.data(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, font_size,
      buffer.data(), &text_format_));

  ThrowIfFailed(text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
  ThrowIfFailed(
      text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}
}  // namespace cru::win::graph
