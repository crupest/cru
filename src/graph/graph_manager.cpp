#include "graph_manager.hpp"

#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

#include "application.hpp"
#include "exception.hpp"
#include "util/com_util.hpp"
#include "window_render_target.hpp"

namespace cru::graph {

GraphManager* GraphManager::GetInstance() {
  return Application::GetInstance()->ResolveSingleton<GraphManager>(
      [](auto) { return new GraphManager{}; });
}

GraphManager::GraphManager() {
  UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef CRU_DEBUG
  creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  const D3D_FEATURE_LEVEL feature_levels[] = {
      D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1};

  Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11_device_context;
  ID3D11Device* d3d11_device;

  ThrowIfFailed(D3D11CreateDevice(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creation_flags,
      feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
      &d3d11_device, nullptr, &d3d11_device_context));
  this->d3d11_device_ = util::CreateComSharedPtr(d3d11_device);

  Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
  ThrowIfFailed(d3d11_device_->QueryInterface(dxgi_device.GetAddressOf()));

  ID2D1Factory1* d2d1_factory;
  ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                  IID_PPV_ARGS(&d2d1_factory)));
  this->d2d1_factory_ = util::CreateComSharedPtr(d2d1_factory);

  Microsoft::WRL::ComPtr<ID2D1Device> d2d1_device;

  ThrowIfFailed(d2d1_factory_->CreateDevice(dxgi_device.Get(), &d2d1_device));

  ID2D1DeviceContext* d2d1_device_context;
  ThrowIfFailed(d2d1_device->CreateDeviceContext(
      D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d1_device_context));
  this->d2d1_device_context_ = util::CreateComSharedPtr(d2d1_device_context);

  // Identify the physical adapter (GPU or card) this device is runs on.
  Microsoft::WRL::ComPtr<IDXGIAdapter> dxgi_adapter;
  ThrowIfFailed(dxgi_device->GetAdapter(&dxgi_adapter));

  IDXGIFactory2* dxgi_factory;
  // Get the factory object that created the DXGI device.
  ThrowIfFailed(dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory)));
  this->dxgi_factory_ = util::CreateComSharedPtr(dxgi_factory);

  IDWriteFactory* dwrite_factory;
  ThrowIfFailed(
      DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                          reinterpret_cast<IUnknown**>(&dwrite_factory)));
  this->dwrite_factory_ = util::CreateComSharedPtr(dwrite_factory);

  IDWriteFontCollection* font_collection;
  ThrowIfFailed(dwrite_factory_->GetSystemFontCollection(&font_collection));
  this->dwrite_system_font_collection_ =
      util::CreateComSharedPtr(font_collection);
}
}  // namespace cru::graph
