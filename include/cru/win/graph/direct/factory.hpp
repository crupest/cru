#pragma once
#include "resource.hpp"

#include "cru/platform/graph/factory.hpp"

namespace cru::platform::graph::win::direct {
class DirectGraphFactory : public DirectResource, public virtual IGraphFactory {
 public:
  DirectGraphFactory();

  CRU_DELETE_COPY(DirectGraphFactory)
  CRU_DELETE_MOVE(DirectGraphFactory)

  ~DirectGraphFactory() override = default;

 public:
  ID2D1Factory1* GetD2D1Factory() const { return d2d1_factory_.Get(); }
  ID2D1DeviceContext* GetD2D1DeviceContext() const {
    return d2d1_device_context_.Get();
  }
  ID3D11Device* GetD3D11Device() const { return d3d11_device_.Get(); }
  IDXGIFactory2* GetDxgiFactory() const { return dxgi_factory_.Get(); }
  IDWriteFactory* GetDWriteFactory() const { return dwrite_factory_.Get(); }
  IDWriteFontCollection* GetSystemFontCollection() const {
    return dwrite_system_font_collection_.Get();
  }

 public:
  std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() override;

  std::unique_ptr<IGeometryBuilder> CreateGeometryBuilder() override;

  std::unique_ptr<IFont> CreateFont(const std::string_view& font_family,
                                    float font_size) override;

  std::unique_ptr<ITextLayout> CreateTextLayout(std::shared_ptr<IFont> font,
                                                std::string text) override;

 private:
  Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device_;
  Microsoft::WRL::ComPtr<ID2D1Factory1> d2d1_factory_;
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context_;
  Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory_;
  Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
  Microsoft::WRL::ComPtr<IDWriteFontCollection> dwrite_system_font_collection_;
};
}  // namespace cru::platform::graph::win::direct
