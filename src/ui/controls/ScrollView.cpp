#include "cru/ui/controls/ScrollView.h"

namespace cru::ui::controls {
ScrollView::ScrollView()
    : SingleChildControl<render::ScrollRenderObject>(kControlName) {}

ScrollView::~ScrollView() {
  RemoveFromParent();
  RemoveAllChild();
}
}  // namespace cru::ui::controls
