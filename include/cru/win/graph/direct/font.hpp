#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/font.hpp"

#include <string_view>

namespace cru::win::graph {
struct IWinNativeFactory;

class WinFontDescriptor : public Object,
                          public virtual platform::graph::IFontDescriptor {
 public:
  explicit WinFontDescriptor(IWinNativeFactory* factory,
                             const std::wstring_view& font_family,
                             float font_size);
  WinFontDescriptor(const WinFontDescriptor& other) = delete;
  WinFontDescriptor(WinFontDescriptor&& other) = delete;
  WinFontDescriptor& operator=(const WinFontDescriptor& other) = delete;
  WinFontDescriptor& operator=(WinFontDescriptor&& other) = delete;
  ~WinFontDescriptor() override = default;

  IDWriteTextFormat* GetDWriteTextFormat() const { return text_format_.Get(); }

 private:
  Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
};
}  // namespace cru::win::graph
