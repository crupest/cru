#pragma once
#include "com_resource.hpp"
#include "direct_factory.hpp"
#include "platform_id.hpp"

#include "cru/platform/graph/font.hpp"

#include <string_view>

namespace cru::platform::graph::win::direct {

class DWriteFont : public Font, public IComResource<IDWriteTextFormat> {
 public:
  DWriteFont(IDirectFactory* factory, const std::wstring_view& font_family,
             float font_size);

  DWriteFont(const DWriteFont& other) = delete;
  DWriteFont& operator=(const DWriteFont& other) = delete;

  DWriteFont(DWriteFont&& other) = delete;
  DWriteFont& operator=(DWriteFont&& other) = delete;

  ~DWriteFont() override = default;

  CRU_PLATFORMID_IMPLEMENT_DIRECT

 public:
  IDWriteTextFormat* GetComInterface() const override { return text_format_.Get(); }

 private:
  Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
};
}  // namespace cru::platform::graph::win::direct
