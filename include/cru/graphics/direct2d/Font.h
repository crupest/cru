#pragma once
#include "ComResource.h"
#include "Resource.h"

#include "cru/graphics/Font.h"

#include <string_view>

namespace cru::graphics::direct2d {
class CRU_WIN_GRAPHICS_DIRECT_API DWriteFont
    : public DirectGraphicsResource,
      public virtual IFont,
      public virtual IComResource<IDWriteTextFormat> {
 public:
  DWriteFont(DirectGraphicsFactory* factory, String font_family,
             float font_size);


  ~DWriteFont() override = default;

 public:
  IDWriteTextFormat* GetComInterface() const override {
    return text_format_.Get();
  }

  String GetFontName() override;
  float GetFontSize() override;

 private:
  String font_family_;
  Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
};
}  // namespace cru::graphics::direct2d
