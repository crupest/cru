#include "cru/win/graph/direct/Font.hpp"

#include "cru/win/graph/direct/Exception.hpp"
#include "cru/win/graph/direct/Factory.hpp"
#include "cru/win/String.hpp"

#include <array>
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

  ThrowIfFailed(text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
  ThrowIfFailed(
      text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
}

float DWriteFont::GetFontSize() { return text_format_->GetFontSize(); }
}  // namespace cru::platform::graph::win::direct
