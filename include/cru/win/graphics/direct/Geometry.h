#pragma once
#include "ComResource.h"
#include "Resource.h"

#include "cru/platform/graphics/Geometry.h"

namespace cru::platform::graphics::win::direct {
class CRU_WIN_GRAPHICS_DIRECT_API D2DGeometryBuilder
    : public DirectGraphicsResource,
      public virtual IGeometryBuilder {
 public:
  explicit D2DGeometryBuilder(DirectGraphicsFactory* factory);

  CRU_DELETE_COPY(D2DGeometryBuilder)
  CRU_DELETE_MOVE(D2DGeometryBuilder)

  ~D2DGeometryBuilder() override = default;

 public:
  Point GetCurrentPosition() override;

  void MoveTo(const Point& point) override;
  void LineTo(const Point& point) override;
  void CubicBezierTo(const Point& start_control_point,
                     const Point& end_control_point,
                     const Point& end_point) override;
  void QuadraticBezierTo(const Point& control_point,
                         const Point& end_point) override;
  void ArcTo(const Point& radius, float angle, bool is_large_arc,
             bool is_clockwise, const Point& end_point) override;
  void CloseFigure(bool close) override;

  std::unique_ptr<IGeometry> Build() override;

 private:
  bool IsValid() { return geometry_ != nullptr; }
  void CheckValidation();

 private:
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry_;
  Microsoft::WRL::ComPtr<ID2D1GeometrySink> geometry_sink_;
  Point start_point_;
  Point current_position_;
};

class CRU_WIN_GRAPHICS_DIRECT_API D2DGeometry
    : public DirectGraphicsResource,
      public virtual IGeometry,
      public IComResource<ID2D1Geometry> {
 public:
  D2DGeometry(DirectGraphicsFactory* factory,
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
}  // namespace cru::platform::graphics::win::direct
