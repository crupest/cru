#pragma once
#include "pre.hpp"

namespace cru::ui {
enum class MouseButton { Left, Right, Middle };

bool IsKeyDown(int virtual_code);
bool IsKeyToggled(int virtual_code);
bool IsAnyMouseButtonDown();
}  // namespace cru::ui
