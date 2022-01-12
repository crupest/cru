#pragma once

#include "cru/common/PreConfig.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef CreateWindow
#undef DrawText
#undef CreateFont
#undef CreateEvent

#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
