#pragma once

#ifdef CRU_IS_DLL
#ifdef CRU_PLATFORM_EXPORT_API
#define CRU_PLATFORM_API __declspec(dllexport)
#else
#define CRU_PLATFORM_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_API
#endif
