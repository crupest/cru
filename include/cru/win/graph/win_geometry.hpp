#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/geometry.hpp"

namespace cru::win::graph {
struct IWinNativeFactory;

class WinGeometryBuilder : public Object,
                           public virtual platform::graph::IGeometryBuilder {
 public:
  explicit WinGeometryBuilder(IWinNativeFactory* factory);
  WinGeometryBuilder(const WinGeometryBuilder& other) = delete;
  WinGeometryBuilder(WinGeometryBuilder&& other) = delete;
  WinGeometryBuilder& operator=(const WinGeometryBuilder& other) = delete;
  WinGeometryBuilder& operator=(WinGeometryBuilder&& other) = delete;
  ~WinGeometryBuilder() override;

  void BeginFigure(const ui::Point& point) override;
  void LineTo(const ui::Point& point) override;
  void QuadraticBezierTo(const ui::Point& control_point,
                         const ui::Point& end_point) override;
  void CloseFigure(bool close) override;
  platform::graph::IGeometry* End() override;
  bool IsEnded() const override { return geometry_ != nullptr; }

 private:
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry_;
  Microsoft::WRL::ComPtr<ID2D1GeometrySink> geometry_sink_;
};

class WinGeometry : public Object, public virtual platform::graph::IGeometry {
 public:
  explicit WinGeometry(Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry);
  WinGeometry(const WinGeometry& other) = delete;
  WinGeometry(WinGeometry&& other) = delete;
  WinGeometry& operator=(const WinGeometry& other) = delete;
  WinGeometry& operator=(WinGeometry&& other) = delete;
  ~WinGeometry() override = default;

  bool FillContains(const ui::Point& point) override;

  ID2D1PathGeometry* GetNative() const { return geometry_.Get(); }

 private:
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry_;
};
}  // namespace cru::win::graph
