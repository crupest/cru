#include "cru/win/graph/direct/Factory.hpp"

#include "cru/common/Logger.hpp"
#include "cru/win/graph/direct/Brush.hpp"
#include "cru/win/graph/direct/Exception.hpp"
#include "cru/win/graph/direct/Font.hpp"
#include "cru/win/graph/direct/Geometry.hpp"
#include "cru/win/graph/direct/TextLayout.hpp"

#include <cstdlib>
#include <utility>

namespace cru::platform::graph::win::direct {
namespace {
void InitializeCom() {
  const auto hresult = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  if (FAILED(hresult)) {
    throw HResultError(hresult, "Failed to call CoInitializeEx.");
  }
  if (hresult == S_FALSE) {
    log::Debug(
        "Try to call CoInitializeEx, but it seems COM is already "
        "initialized.");
  }
}

void UninitializeCom() { ::CoUninitialize(); }
}  // namespace

DirectGraphFactory::DirectGraphFactory() {
  // TODO! Detect repeated creation. Because I don't think we can create two d2d
  // and dwrite factory so we need to prevent the "probably dangerous" behavior.

  InitializeCom();

  UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef CRU_DEBUG
  creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  const D3D_FEATURE_LEVEL feature_levels[] = {
      D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1};

  Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11_device_context;

  ThrowIfFailed(D3D11CreateDevice(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creation_flags,
      feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
      &d3d11_device_, nullptr, &d3d11_device_context));

  Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
  ThrowIfFailed(d3d11_device_->QueryInterface(dxgi_device.GetAddressOf()));

  ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                  IID_PPV_ARGS(&d2d1_factory_)));

  ThrowIfFailed(d2d1_factory_->CreateDevice(dxgi_device.Get(), &d2d1_device_));

  d2d1_device_context_ = CreateD2D1DeviceContext();

  // Identify the physical adapter (GPU or card) this device is runs on.
  Microsoft::WRL::ComPtr<IDXGIAdapter> dxgi_adapter;
  ThrowIfFailed(dxgi_device->GetAdapter(&dxgi_adapter));

  // Get the factory object that created the DXGI device.
  ThrowIfFailed(dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory_)));

  ThrowIfFailed(DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
      reinterpret_cast<IUnknown**>(dwrite_factory_.GetAddressOf())));

  ThrowIfFailed(dwrite_factory_->GetSystemFontCollection(
      &dwrite_system_font_collection_));
}

DirectGraphFactory::~DirectGraphFactory() { UninitializeCom(); }

Microsoft::WRL::ComPtr<ID2D1DeviceContext>
DirectGraphFactory::CreateD2D1DeviceContext() {
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context;
  ThrowIfFailed(d2d1_device_->CreateDeviceContext(
      D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d1_device_context));
  return d2d1_device_context;
}

std::unique_ptr<ISolidColorBrush> DirectGraphFactory::CreateSolidColorBrush() {
  return std::make_unique<D2DSolidColorBrush>(this);
}

std::unique_ptr<IGeometryBuilder> DirectGraphFactory::CreateGeometryBuilder() {
  return std::make_unique<D2DGeometryBuilder>(this);
}

std::unique_ptr<IFont> DirectGraphFactory::CreateFont(
    const std::string_view& font_family, float font_size) {
  return std::make_unique<DWriteFont>(this, font_family, font_size);
}

std::unique_ptr<ITextLayout> DirectGraphFactory::CreateTextLayout(
    std::shared_ptr<IFont> font, std::string text) {
  return std::make_unique<DWriteTextLayout>(this, std::move(font),
                                            std::move(text));
}
}  // namespace cru::platform::graph::win::direct
