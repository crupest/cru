#include "cru/win/graph/direct/Font.hpp"

#include "cru/win/graph/direct/Exception.hpp"
#include "cru/win/graph/direct/Factory.hpp"

#include <array>
#include <utility>

namespace cru::platform::graph::win::direct {
DWriteFont::DWriteFont(DirectGraphFactory* factory, std::u16string font_family,
                       float font_size)
    : DirectGraphResource(factory), font_family_(std::move(font_family)) {
  // Get locale
  std::array<wchar_t, LOCALE_NAME_MAX_LENGTH> buffer;
  if (!::GetUserDefaultLocaleName(buffer.data(),
                                  static_cast<int>(buffer.size())))
    throw platform::win::Win32Error(
        ::GetLastError(), "Failed to get locale when create dwrite font.");

  ThrowIfFailed(factory->GetDWriteFactory()->CreateTextFormat(
      reinterpret_cast<const wchar_t*>(font_family_.c_str()), nullptr,
      DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL, font_size, buffer.data(), &text_format_));

  ThrowIfFailed(text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
  ThrowIfFailed(
      text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
}

float DWriteFont::GetFontSize() { return text_format_->GetFontSize(); }
}  // namespace cru::platform::graph::win::direct
