#pragma once
#include "com_resource.hpp"
#include "direct_factory.hpp"

#include "cru/platform/graph/brush.hpp"

namespace cru::platform::graph::win::direct {
class D2DSolidColorBrush : public SolidColorBrush,
                           public IComResource<ID2D1SolidColorBrush> {
 public:
  explicit D2DSolidColorBrush(IDirectFactory* factory);
  D2DSolidColorBrush(const D2DSolidColorBrush& other) = delete;
  D2DSolidColorBrush(D2DSolidColorBrush&& other) = delete;
  D2DSolidColorBrush& operator=(const D2DSolidColorBrush& other) = delete;
  D2DSolidColorBrush& operator=(D2DSolidColorBrush&& other) = delete;
  ~D2DSolidColorBrush() override = default;

  ID2D1SolidColorBrush* GetComInterface() override { return brush_.Get(); }

 protected:
  void OnSetColor(const Color& color) override;

 private:
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush_;
};
}  // namespace cru::platform::graph::win::direct
