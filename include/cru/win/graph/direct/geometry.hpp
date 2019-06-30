#pragma once
#include "com_resource.hpp"
#include "direct_factory.hpp"
#include "platform_id.hpp"

#include "cru/platform/exception.hpp"
#include "cru/platform/graph/geometry.hpp"

namespace cru::platform::graph::win::direct {
class D2DGeometryBuilder : public GeometryBuilder {
 public:
  explicit D2DGeometryBuilder(IDirectFactory* factory);

  D2DGeometryBuilder(const D2DGeometryBuilder& other) = delete;
  D2DGeometryBuilder& operator=(const D2DGeometryBuilder& other) = delete;

  D2DGeometryBuilder(D2DGeometryBuilder&& other) = delete;
  D2DGeometryBuilder& operator=(D2DGeometryBuilder&& other) = delete;

  ~D2DGeometryBuilder() override = default;

  CRU_PLATFORMID_IMPLEMENT_DIRECT

 public:
  void BeginFigure(const Point& point) override;
  void LineTo(const Point& point) override;
  void QuadraticBezierTo(const Point& control_point,
                         const Point& end_point) override;
  void CloseFigure(bool close) override;

  Geometry* Build() override;

 private:
  bool IsValid() { return geometry_ != nullptr; }
  void CheckValidation() {
    if (!IsValid())
      throw ReuseException("The geometry builder is already disposed.");
  }

 private:
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry_;
  Microsoft::WRL::ComPtr<ID2D1GeometrySink> geometry_sink_;
};

class D2DGeometry : public Geometry, public IComResource<ID2D1Geometry> {
 public:
  explicit D2DGeometry(Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry);

  D2DGeometry(const D2DGeometry& other) = delete;
  D2DGeometry& operator=(const D2DGeometry& other) = delete;

  D2DGeometry(D2DGeometry&& other) = delete;
  D2DGeometry& operator=(D2DGeometry&& other) = delete;

  ~D2DGeometry() override = default;

  CRU_PLATFORMID_IMPLEMENT_DIRECT

 public:
  ID2D1Geometry* GetComInterface() const override { return geometry_.Get(); }

 public:
  bool FillContains(const Point& point) override;

 private:
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry_;
};
}  // namespace cru::platform::graph::win::direct
