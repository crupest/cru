#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

//include system headers

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#pragma comment(lib, "D3D11.lib")
#include <d3d11.h>

#pragma comment(lib, "D2d1.lib")
#include <d2d1_1.h>

#pragma comment(lib, "DWrite.lib")
#include <dwrite.h>

#include <dxgi1_2.h>
#include <wrl/client.h>

#include <VersionHelpers.h>
