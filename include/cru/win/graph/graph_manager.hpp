#pragma once
#include "../win_pre_config.hpp"

#include "cru/common/base.hpp"
#include "cru/win/graph/win_graph_factory.hpp"

#include <memory>

namespace cru::win::graph {
class GraphManager final : public Object {
 public:
  static GraphManager* GetInstance();

 public:
  GraphManager();
  GraphManager(const GraphManager& other) = delete;
  GraphManager(GraphManager&& other) = delete;
  GraphManager& operator=(const GraphManager& other) = delete;
  GraphManager& operator=(GraphManager&& other) = delete;
  ~GraphManager() override = default;

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

  WinGraphFactory* GetGraphFactory() const { return graph_factory_.get(); }

 private:
  Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device_;
  Microsoft::WRL::ComPtr<ID2D1Factory1> d2d1_factory_;
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context_;
  Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory_;
  Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
  Microsoft::WRL::ComPtr<IDWriteFontCollection> dwrite_system_font_collection_;

  std::unique_ptr<WinGraphFactory> graph_factory_;
};
}  // namespace cru::win::graph
