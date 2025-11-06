#pragma once

#include <cru/base/platform/win/Base.h>
#include <cru/platform/graphics/Base.h>

#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

#ifdef CRU_IS_DLL
#ifdef CRU_WIN_GRAPHICS_DIRECT_EXPORT_API
#define CRU_WIN_GRAPHICS_DIRECT_API __declspec(dllexport)
#else
#define CRU_WIN_GRAPHICS_DIRECT_API __declspec(dllimport)
#endif
#else
#define CRU_WIN_GRAPHICS_DIRECT_API
#endif

namespace cru::platform::graphics::direct2d {
using platform::win::HResultError;
using platform::win::ThrowIfFailed;

inline D2D1_MATRIX_3X2_F Convert(const platform::Matrix& matrix) {
  D2D1_MATRIX_3X2_F m;
  m._11 = matrix.m11;
  m._12 = matrix.m12;
  m._21 = matrix.m21;
  m._22 = matrix.m22;
  m._31 = matrix.m31;
  m._32 = matrix.m32;
  return m;
}

inline D2D1_COLOR_F Convert(const Color& color) {
  return D2D1::ColorF(color.red / 255.0f, color.green / 255.0f,
                      color.blue / 255.0f, color.alpha / 255.0f);
}

inline D2D1_POINT_2F Convert(const Point& point) {
  return D2D1::Point2F(point.x, point.y);
}

inline D2D1_RECT_F Convert(const Rect& rect) {
  return D2D1::RectF(rect.left, rect.top, rect.left + rect.width,
                     rect.top + rect.height);
}

inline D2D1_ROUNDED_RECT Convert(const RoundedRect& rounded_rect) {
  return D2D1::RoundedRect(Convert(rounded_rect.rect), rounded_rect.radius_x,
                           rounded_rect.radius_y);
}

inline D2D1_ELLIPSE Convert(const Ellipse& ellipse) {
  return D2D1::Ellipse(Convert(ellipse.center), ellipse.radius_x,
                       ellipse.radius_y);
}

inline platform::Matrix Convert(const D2D1_MATRIX_3X2_F& matrix) {
  return platform::Matrix{matrix._11, matrix._12, matrix._21,
                          matrix._22, matrix._31, matrix._32};
}

inline Color Convert(const D2D1_COLOR_F& color) {
  auto floor = [](float n) { return static_cast<std::uint8_t>(n + 0.5f); };
  return Color{floor(color.r * 255.0f), floor(color.g * 255.0f),
               floor(color.b * 255.0f), floor(color.a * 255.0f)};
}

inline Point Convert(const D2D1_POINT_2F& point) {
  return Point(point.x, point.y);
}

inline Rect Convert(const D2D1_RECT_F& rect) {
  return Rect(rect.left, rect.top, rect.right - rect.left,
              rect.bottom - rect.top);
}

inline RoundedRect Convert(const D2D1_ROUNDED_RECT& rounded_rect) {
  return RoundedRect(Convert(rounded_rect.rect), rounded_rect.radiusX,
                     rounded_rect.radiusY);
}

inline Ellipse Convert(const D2D1_ELLIPSE& ellipse) {
  return Ellipse(Convert(ellipse.point), ellipse.radiusX, ellipse.radiusY);
}

inline bool operator==(const D2D1_POINT_2F& left, const D2D1_POINT_2F& right) {
  return left.x == right.x && left.y == right.y;
}

inline bool operator!=(const D2D1_POINT_2F& left, const D2D1_POINT_2F& right) {
  return !(left == right);
}

inline bool operator==(const D2D1_RECT_F& left, const D2D1_RECT_F& right) {
  return left.left == right.left && left.top == right.top &&
         left.right == right.right && left.bottom == right.bottom;
}

inline bool operator!=(const D2D1_RECT_F& left, const D2D1_RECT_F& right) {
  return !(left == right);
}

inline bool operator==(const D2D1_ROUNDED_RECT& left,
                       const D2D1_ROUNDED_RECT& right) {
  return left.rect == right.rect && left.radiusX == right.radiusX &&
         left.radiusY == right.radiusY;
}

inline bool operator!=(const D2D1_ROUNDED_RECT& left,
                       const D2D1_ROUNDED_RECT& right) {
  return !(left == right);
}

inline bool operator==(const D2D1_ELLIPSE& left, const D2D1_ELLIPSE& right) {
  return left.point == right.point && left.radiusX == right.radiusX &&
         left.radiusY == right.radiusY;
}

inline bool operator!=(const D2D1_ELLIPSE& left, const D2D1_ELLIPSE& right) {
  return !(left == right);
}

class DirectGraphicsFactory;

class CRU_WIN_GRAPHICS_DIRECT_API DirectGraphicsResource
    : public Object,
      public virtual IGraphicsResource {
 public:
  constexpr static auto kPlatformId = "Windows Direct";

 protected:
  // Param factory can't be null.
  explicit DirectGraphicsResource(DirectGraphicsFactory* factory);

 public:
  std::string GetPlatformId() const override;

  IGraphicsFactory* GetGraphicsFactory() final;

 public:
  DirectGraphicsFactory* GetDirectFactory() const { return factory_; }

 private:
  DirectGraphicsFactory* factory_;
};

template <typename TInterface>
struct CRU_WIN_GRAPHICS_DIRECT_API IComResource : virtual Interface {
  virtual TInterface* GetComInterface() const = 0;
};
}  // namespace cru::platform::graphics::direct2d
