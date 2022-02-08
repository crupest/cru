#include "cru/ui/host/LayoutPaintCycler.h"
#include <chrono>

#include "../Helper.h"
#include "cru/ui/Base.h"
#include "cru/ui/host/WindowHost.h"

namespace cru::ui::host {
LayoutPaintCycler::LayoutPaintCycler(WindowHost* host) : host_(host) {
  timer_canceler_ = GetUiApplication()->SetInterval(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          this->cycle_threshold_),
      [this] { OnCycle(); });
}

LayoutPaintCycler::~LayoutPaintCycler() = default;

void LayoutPaintCycler::InvalidateLayout() { layout_dirty_ = true; }

void LayoutPaintCycler::InvalidatePaint() { paint_dirty_ = true; }

void LayoutPaintCycler::OnCycle() {
  last_cycle_time_ = std::chrono::steady_clock::now();
  if (layout_dirty_) {
    host_->Relayout();
    host_->Repaint();
  } else {
    if (paint_dirty_) {
      host_->Repaint();
    }
  }
  layout_dirty_ = false;
  paint_dirty_ = false;
}
}  // namespace cru::ui::host
