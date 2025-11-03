#pragma once

#include <cru/platform/graphics/Base.h>

#include <cairo/cairo.h>

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API
#define CRU_PLATFORM_GRAPHICS_CAIRO_API __declspec(dllexport)
#else
#define CRU_PLATFORM_GRAPHICS_CAIRO_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_GRAPHICS_CAIRO_API
#endif

namespace cru::platform::graphics::cairo {
cairo_matrix_t CRU_PLATFORM_GRAPHICS_CAIRO_API Convert(const Matrix& matrix);
Matrix CRU_PLATFORM_GRAPHICS_CAIRO_API Convert(const cairo_matrix_t* matrix);
Matrix CRU_PLATFORM_GRAPHICS_CAIRO_API Convert(const cairo_matrix_t& matrix);

class CairoGraphicsFactory;

class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoResource
    : public Object,
      public virtual IGraphicsResource {
 public:
  explicit CairoResource(CairoGraphicsFactory* factory);
  ~CairoResource() override;

  std::string GetPlatformId() const override;
  IGraphicsFactory* GetGraphicsFactory() override;
  CairoGraphicsFactory* GetCairoGraphicsFactory() const { return factory_; }

 private:
  CairoGraphicsFactory* factory_;
};
}  // namespace cru::platform::graphics::cairo
