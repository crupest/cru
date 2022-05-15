#pragma once
#include "Resource.h"

#include "ImageFactory.h"

#include "cru/common/platform/win/ComAutoInit.h"
#include "cru/platform/graphics/Base.h"
#include "cru/platform/graphics/Factory.h"

namespace cru::platform::graphics::direct2d {
class CRU_WIN_GRAPHICS_DIRECT_API DirectGraphicsFactory
    : public DirectResource,
      public virtual IGraphicsFactory {
 public:
  DirectGraphicsFactory();

  CRU_DELETE_COPY(DirectGraphicsFactory)
  CRU_DELETE_MOVE(DirectGraphicsFactory)

  ~DirectGraphicsFactory() override;

 public:
  ID3D11Device* GetD3D11Device() const { return d3d11_device_.Get(); }
  ID2D1Factory2* GetD2D1Factory() const { return d2d1_factory_.Get(); }
  ID2D1Device1* GetD2D1Device() const { return d2d1_device_.Get(); }
  IDXGIFactory2* GetDxgiFactory() const { return dxgi_factory_.Get(); }
  IDWriteFactory* GetDWriteFactory() const { return dwrite_factory_.Get(); }
  IDWriteFontCollection* GetSystemFontCollection() const {
    return dwrite_system_font_collection_.Get();
  }

 public:
  Microsoft::WRL::ComPtr<ID2D1DeviceContext1> CreateD2D1DeviceContext();

  // This context should only be used to create graphic resources like brush.
  // Because graphic resources can be shared if they are created in the same
  // device.
  ID2D1DeviceContext* GetDefaultD2D1DeviceContext() {
    return d2d1_device_context_.Get();
  }

 public:
  std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() override;

  std::unique_ptr<IGeometryBuilder> CreateGeometryBuilder() override;

  std::unique_ptr<IFont> CreateFont(String font_family,
                                    float font_size) override;

  std::unique_ptr<ITextLayout> CreateTextLayout(std::shared_ptr<IFont> font,
                                                String text) override;

  IImageFactory* GetImageFactory() override;

 private:
  platform::win::ComAutoInit com_auto_init_;

  Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device_;
  Microsoft::WRL::ComPtr<ID2D1Factory2> d2d1_factory_;
  Microsoft::WRL::ComPtr<ID2D1Device1> d2d1_device_;
  Microsoft::WRL::ComPtr<ID2D1DeviceContext1> d2d1_device_context_;
  Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory_;
  Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
  Microsoft::WRL::ComPtr<IDWriteFontCollection> dwrite_system_font_collection_;

  std::unique_ptr<WinImageFactory> image_factory_;
};
}  // namespace cru::platform::graphics::direct2d
