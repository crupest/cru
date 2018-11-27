#pragma once

#ifdef _DEBUG
#define CRU_DEBUG
#endif

#ifdef CRU_DEBUG
#define CRU_DEBUG_LAYOUT
#endif


#ifdef CRU_DEBUG
// ReSharper disable once IdentifierTypo
// ReSharper disable once CppInconsistentNaming
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif
