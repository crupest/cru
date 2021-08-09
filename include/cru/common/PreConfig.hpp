#pragma once

#ifdef _MSC_VER
// disable the unnecessary warning about multi-inheritance
#pragma warning(disable : 4250)
// disable dll export template issue warning
#pragma warning(disable : 4251)
#endif

#ifdef CRU_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
