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
