#pragma once
#include "cru/platform/win/WinPreConfig.h"

#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_WIN_GRAPHICS_DIRECT_EXPORT_API
#define CRU_WIN_GRAPHICS_DIRECT_API __declspec(dllexport)
#else
#define CRU_WIN_GRAPHICS_DIRECT_API __declspec(dllimport)
#endif
#else
#define CRU_WIN_GRAPHICS_DIRECT_API
#endif
