#pragma once

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_TOML_EXPORT_API
#define CRU_TOML_API __declspec(dllexport)
#else
#define CRU_TOML_API __declspec(dllimport)
#endif
#else
#define CRU_TOML_API
#endif
