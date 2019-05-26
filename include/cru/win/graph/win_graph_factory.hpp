#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/graph_factory.hpp"
#include "win_native_factory.hpp"

namespace cru::win::graph {
class WinGraphFactory : public Object,
                        public virtual platform::graph::IGraphFactory,
                        public virtual IWinNativeFactory {
  friend IGraphFactory* IGraphFactory::CreateInstance();

 public:
  static WinGraphFactory* GetInstance();

 private:
  explicit WinGraphFactory();

 public:
  WinGraphFactory(const WinGraphFactory& other) = delete;
  WinGraphFactory(WinGraphFactory&& other) = delete;
  WinGraphFactory& operator=(const WinGraphFactory& other) = delete;
  WinGraphFactory& operator=(WinGraphFactory&& other) = delete;
  ~WinGraphFactory() override;

  ID2D1Factory1* GetD2D1Factory() const override { return d2d1_factory_.Get(); }
  ID2D1DeviceContext* GetD2D1DeviceContext() const override {
    return d2d1_device_context_.Get();
  }
  ID3D11Device* GetD3D11Device() const override { return d3d11_device_.Get(); }
  IDXGIFactory2* GetDxgiFactory() const override { return dxgi_factory_.Get(); }
  IDWriteFactory* GetDWriteFactory() const override {
    return dwrite_factory_.Get();
  }
  IDWriteFontCollection* GetSystemFontCollection() const override {
    return dwrite_system_font_collection_.Get();
  }

  platform::graph::ISolidColorBrush* CreateSolidColorBrush(
      const ui::Color& color) override;
  platform::graph::IGeometryBuilder* CreateGeometryBuilder() override;
  platform::graph::IFontDescriptor* CreateFontDescriptor(
      const std::wstring_view& font_family, float font_size);
  platform::graph::ITextLayout* CreateTextLayout(
      std::shared_ptr<platform::graph::IFontDescriptor> font,
      std::wstring text);

  bool IsAutoDelete() const override { return auto_delete_; }
  void SetAutoDelete(bool value) override { auto_delete_ = value; }

 private:
  bool auto_delete_ = false;

  Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device_;
  Microsoft::WRL::ComPtr<ID2D1Factory1> d2d1_factory_;
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context_;
  Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory_;
  Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
  Microsoft::WRL::ComPtr<IDWriteFontCollection> dwrite_system_font_collection_;
};
}  // namespace cru::win::graph
