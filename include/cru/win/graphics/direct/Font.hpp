#pragma once
#include "ComResource.hpp"
#include "Resource.hpp"

#include "cru/platform/graphics/Font.hpp"

#include <string_view>

namespace cru::platform::graphics::win::direct {
class DWriteFont : public DirectGraphResource,
                   public virtual IFont,
                   public virtual IComResource<IDWriteTextFormat> {
 public:
  DWriteFont(DirectGraphFactory* factory, std::u16string font_family,
             float font_size);

  CRU_DELETE_COPY(DWriteFont)
  CRU_DELETE_MOVE(DWriteFont)

  ~DWriteFont() override = default;

 public:
  IDWriteTextFormat* GetComInterface() const override {
    return text_format_.Get();
  }

  float GetFontSize() override;

 private:
  std::u16string font_family_;
  Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
};
}  // namespace cru::platform::graphics::win::direct
