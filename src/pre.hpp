#pragma once

#ifdef _DEBUG
#define CRU_DEBUG
#endif

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#ifdef CRU_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#endif
