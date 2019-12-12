#include "cru/win/graph/direct/font.hpp"

#include "cru/win/graph/direct/exception.hpp"
#include "cru/win/graph/direct/factory.hpp"
#include "cru/win/string.hpp"

#include <array>
#include <cassert>
#include <utility>

namespace cru::platform::graph::win::direct {
DWriteFont::DWriteFont(DirectGraphFactory* factory,
                       const std::string_view& font_family, float font_size)
    : DirectGraphResource(factory) {
  // Get locale
  std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> buffer;
  if (!::GetUserDefaultLocaleName(buffer.data(),
                                  static_cast<int>(buffer.size())))
    throw platform::win::Win32Error(
        ::GetLastError(), "Failed to get locale when create dwrite font.");

  const std::wstring&& wff = cru::platform::win::ToUtf16String(font_family);

  ThrowIfFailed(factory->GetDWriteFactory()->CreateTextFormat(
      wff.data(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL, font_size, buffer.data(), &text_format_));

  ThrowIfFailed(text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
  ThrowIfFailed(
      text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}
}  // namespace cru::platform::graph::win::direct
