#pragma once

#ifdef _DEBUG
#define CRU_DEBUG
#endif

#ifdef CRU_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#endif
