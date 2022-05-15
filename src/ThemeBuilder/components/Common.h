#pragma once
#include "cru/platform/graphics/Brush.h"

namespace cru::theme_builder::components {
std::unique_ptr<platform::graphics::ISolidColorBrush>
CreateRandomEditorBackgroundBrush();
}
