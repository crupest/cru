#pragma once
#include "win_pre_config.hpp"

#include "../text_layout.hpp"
#include "win_font.hpp"

#include <memory>

namespace cru::platform::win {
class WinTextLayout : public Object, public virtual TextLayout {
 public:
  explicit WinTextLayout(std::shared_ptr<WinFontDescriptor> font);
  WinTextLayout(const WinTextLayout& other) = delete;
  WinTextLayout(WinTextLayout&& other) = delete;
  WinTextLayout& operator=(const WinTextLayout& other) = delete;
  WinTextLayout& operator=(WinTextLayout&& other) = delete;
  ~WinTextLayout() override;

  IDWriteTextLayout* GetDWriteTextLayout() const { return text_layout_.Get(); }

 private:
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;
};
}  // namespace cru::platform::win
