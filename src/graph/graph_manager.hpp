#pragma once
#include "pre.hpp"

#include <memory>

#include "base.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID2D1Factory1;
struct ID2D1DeviceContext;
struct IDXGIFactory2;
struct IDWriteFontCollection;
struct IDWriteFactory;

struct ID2D1RenderTarget;

namespace cru::graph {
class WindowRenderTarget;

class GraphManager final : public Object {
 public:
  static GraphManager* GetInstance();

 private:
  GraphManager();

 public:
  GraphManager(const GraphManager& other) = delete;
  GraphManager(GraphManager&& other) = delete;
  GraphManager& operator=(const GraphManager& other) = delete;
  GraphManager& operator=(GraphManager&& other) = delete;
  ~GraphManager() override = default;

 public:
  ID2D1Factory1* GetD2D1Factory() const { return d2d1_factory_.get(); }

  ID2D1DeviceContext* GetD2D1DeviceContext() const {
    return d2d1_device_context_.get();
  }

  ID3D11Device* GetD3D11Device() const { return d3d11_device_.get(); }

  IDXGIFactory2* GetDxgiFactory() const { return dxgi_factory_.get(); }

  IDWriteFactory* GetDWriteFactory() const { return dwrite_factory_.get(); }

  IDWriteFontCollection* GetSystemFontCollection() const {
    return dwrite_system_font_collection_.get();
  }

 private:
  std::shared_ptr<ID3D11Device> d3d11_device_;
  std::shared_ptr<ID2D1Factory1> d2d1_factory_;
  std::shared_ptr<ID2D1DeviceContext> d2d1_device_context_;
  std::shared_ptr<IDXGIFactory2> dxgi_factory_;
  std::shared_ptr<IDWriteFactory> dwrite_factory_;
  std::shared_ptr<IDWriteFontCollection> dwrite_system_font_collection_;
};
}  // namespace cru::graph
