#pragma once
#include "direct_factory.hpp"

#include "brush.hpp"

#include "cru/platform/graph/graph_factory.hpp"

namespace cru::platform::graph::win::direct {
class DirectGraphFactory : public GraphFactory, IDirectFactory {
  friend GraphFactory* GraphFactory::CreateInstance();

 public:
  static DirectGraphFactory* GetInstance();

 private:
  DirectGraphFactory();

 public:
  DirectGraphFactory(const DirectGraphFactory& other) = delete;
  DirectGraphFactory(DirectGraphFactory&& other) = delete;
  DirectGraphFactory& operator=(const DirectGraphFactory& other) = delete;
  DirectGraphFactory& operator=(DirectGraphFactory&& other) = delete;
  ~DirectGraphFactory() override;

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

  D2DSolidColorBrush* CreateSolidColorBrush() override;

  D2DGeometryBuilder* CreateGeometryBuilder() override;
  D2DFont* CreateFont(
      const std::wstring_view& font_family, float font_size) override;
  DWriteTextLayout* CreateTextLayout(
      std::shared_ptr<Font> font,
      std::wstring text) override;

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
