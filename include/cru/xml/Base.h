#pragma once

#ifdef CRU_IS_DLL
#ifdef CRU_XML_EXPORT_API
#define CRU_XML_API __declspec(dllexport)
#else
#define CRU_XML_API __declspec(dllimport)
#endif
#else
#define CRU_XML_API
#endif
