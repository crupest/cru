#include "cru/ui/controls/container.hpp"

#include "cru/platform/graph/factory.hpp"
#include "cru/ui/render/border_render_object.hpp"

namespace cru::ui::controls {
Container::Container() {
  render_object_ = std::make_unique<render::BorderRenderObject>();
  render_object_->SetBorderEnabled(false);
}

Container::~Container() {}
}  // namespace cru::ui::controls
