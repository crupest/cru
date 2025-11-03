#pragma once
#include <cru/platform/Base.h>      // IWYU pragma: export

#ifdef CRU_IS_DLL
#ifdef CRU_PLATFORM_GUI_EXPORT_API
#define CRU_PLATFORM_GUI_API __declspec(dllexport)
#else
#define CRU_PLATFORM_GUI_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_GUI_API
#endif
