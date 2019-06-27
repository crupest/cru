#pragma once
#include "com_resource.hpp"
#include "direct_factory.hpp"
#include "platform_id.hpp"

#include "cru/platform/graph/brush.hpp"

namespace cru::platform::graph::win::direct {
struct ID2DBrush {
  virtual ~ID2DBrush() = default;

  virtual ID2D1Brush* GetD2DBrushInterface() const = 0;
};

class D2DSolidColorBrush : public SolidColorBrush,
                           public ID2DBrush,
                           public IComResource<ID2D1SolidColorBrush> {
 public:
  explicit D2DSolidColorBrush(IDirectFactory* factory);

  D2DSolidColorBrush(const D2DSolidColorBrush& other) = delete;
  D2DSolidColorBrush& operator=(const D2DSolidColorBrush& other) = delete;

  D2DSolidColorBrush(D2DSolidColorBrush&& other) = delete;
  D2DSolidColorBrush& operator=(D2DSolidColorBrush&& other) = delete;

  ~D2DSolidColorBrush() override = default;

  CRU_PLATFORMID_IMPLEMENT_DIRECT

 public:
  ID2D1Brush* GetD2DBrushInterface() const override { return brush_.Get(); }

  ID2D1SolidColorBrush* GetComInterface() const override {
    return brush_.Get();
  }

 protected:
  void OnSetColor(const Color& color) override;

 private:
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush_;
};
}  // namespace cru::platform::graph::win::direct
