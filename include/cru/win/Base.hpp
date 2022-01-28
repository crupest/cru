#pragma once

#include "WinPreConfig.hpp"

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_WIN_EXPORT_API
#define CRU_WIN_API __declspec(dllexport)
#else
#define CRU_WIN_API __declspec(dllimport)
#endif
#else
#define CRU_WIN_API
#endif
