#pragma once

#ifdef CRU_IS_DLL
#ifdef CRU_PARSE_EXPORT_API
#define CRU_PARSE_API __declspec(dllexport)
#else
#define CRU_PARSE_API __declspec(dllimport)
#endif
#else
#define CRU_PARSE_API
#endif
