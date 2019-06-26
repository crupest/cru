#pragma once
#include "../graphic_base.hpp"
#include "../native_resource.hpp"

namespace cru::platform::graph {
class Geometry : public NativeResource {
 protected:
  Geometry() = default;

 public:
  Geometry(const Geometry& other) = delete;
  Geometry& operator=(const Geometry& other) = delete;

  Geometry(Geometry&& other) = delete;
  Geometry& operator=(Geometry&& other) = delete;

  ~Geometry() override = default;

 public:
  virtual bool FillContains(const Point& point) = 0;
};

class GeometryBuilder : public NativeResource {
 protected:
  GeometryBuilder() = default;

 public:
  GeometryBuilder(const GeometryBuilder& other) = delete;
  GeometryBuilder& operator=(const GeometryBuilder& other) = delete;

  GeometryBuilder(GeometryBuilder&& other) = delete;
  GeometryBuilder& operator=(GeometryBuilder&& other) = delete;

  ~GeometryBuilder() override = default;

 public:
  virtual void BeginFigure(const Point& point) = 0;
  virtual void LineTo(const Point& point) = 0;
  virtual void QuadraticBezierTo(const Point& control_point,
                                 const Point& end_point) = 0;
  virtual void CloseFigure(bool close) = 0;

  virtual Geometry* Build() = 0;
};
}  // namespace cru::platform::graph
