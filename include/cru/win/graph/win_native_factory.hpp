#pragma once
#include "../win_pre_config.hpp"

#include "cru/common/base.hpp"

namespace cru::win::graph {
// Interface provides access to root d2d resources.
struct IWinNativeFactory : virtual Interface {
  virtual ID2D1Factory1* GetD2D1Factory() const = 0;
  virtual ID2D1DeviceContext* GetD2D1DeviceContext() const = 0;
  virtual ID3D11Device* GetD3D11Device() const = 0;
  virtual IDXGIFactory2* GetDxgiFactory() const = 0;
  virtual IDWriteFactory* GetDWriteFactory() const = 0;
  virtual IDWriteFontCollection* GetSystemFontCollection() const = 0;
};
}
