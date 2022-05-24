#pragma once

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_PLATFORM_GRAPHICS_CAIRO_EXPORT_API
#define CRU_PLATFORM_GRAPHICS_CAIRO_API __declspec(dllexport)
#else
#define CRU_PLATFORM_GRAPHICS_CAIRO_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_GRAPHICS_CAIRO_API
#endif

#include <cairo/cairo.h>

#include "../Base.h"

namespace cru::platform::graphics::cairo {
cairo_matrix_t CRU_PLATFORM_GRAPHICS_CAIRO_API Convert(const Matrix& matrix);
Matrix CRU_PLATFORM_GRAPHICS_CAIRO_API Convert(const cairo_matrix_t* matrix);
Matrix CRU_PLATFORM_GRAPHICS_CAIRO_API Convert(const cairo_matrix_t& matrix);
}  // namespace cru::platform::graphics::cairo
