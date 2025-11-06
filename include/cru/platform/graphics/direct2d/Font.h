#pragma once
#include "Base.h"

#include <cru/platform/graphics/Font.h>

namespace cru::platform::graphics::direct2d {
class CRU_WIN_GRAPHICS_DIRECT_API DWriteFont
    : public DirectGraphicsResource,
      public virtual IFont,
      public virtual IComResource<IDWriteTextFormat> {
 public:
  DWriteFont(DirectGraphicsFactory* factory, std::string font_family,
             float font_size);

  IDWriteTextFormat* GetComInterface() const override {
    return text_format_.Get();
  }

  std::string GetFontName() override;
  float GetFontSize() override;

 private:
  std::string font_family_;
  Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
};
}  // namespace cru::platform::graphics::direct2d
