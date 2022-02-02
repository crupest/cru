#pragma once
#include "../../PreConfig.hpp"
#ifdef CRU_PLATFORM_WINDOWS

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef CreateWindow
#undef DrawText
#undef CreateFont
#undef CreateEvent

#endif
