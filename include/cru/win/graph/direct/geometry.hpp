#pragma once
#include "com_resource.hpp"
#include "resource.hpp"

#include "cru/platform/graph/geometry.hpp"

namespace cru::platform::graph::win::direct {
class D2DGeometryBuilder : public DirectGraphResource,
                           public virtual IGeometryBuilder {
 public:
  explicit D2DGeometryBuilder(DirectGraphFactory* factory);

  CRU_DELETE_COPY(D2DGeometryBuilder)
  CRU_DELETE_MOVE(D2DGeometryBuilder)

  ~D2DGeometryBuilder() override = default;

 public:
  void BeginFigure(const Point& point) override;
  void LineTo(const Point& point) override;
  void QuadraticBezierTo(const Point& control_point,
                         const Point& end_point) override;
  void CloseFigure(bool close) override;

  std::unique_ptr<IGeometry> Build() override;

 private:
  bool IsValid() { return geometry_ != nullptr; }
  void CheckValidation();

 private:
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry_;
  Microsoft::WRL::ComPtr<ID2D1GeometrySink> geometry_sink_;
};

class D2DGeometry : public DirectGraphResource,
                    public virtual IGeometry,
                    public IComResource<ID2D1Geometry> {
 public:
  D2DGeometry(DirectGraphFactory* factory,
              Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry);

  CRU_DELETE_COPY(D2DGeometry)
  CRU_DELETE_MOVE(D2DGeometry)

  ~D2DGeometry() override = default;

 public:
  ID2D1Geometry* GetComInterface() const override { return geometry_.Get(); }

 public:
  bool FillContains(const Point& point) override;

 private:
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry_;
};
}  // namespace cru::platform::graph::win::direct
