#pragma once
#include "cru/graphics/Brush.h"

namespace cru::theme_builder::components {
std::unique_ptr<graphics::ISolidColorBrush>
CreateRandomEditorBackgroundBrush();
}
