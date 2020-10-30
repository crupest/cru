#include "cru/ui/host/LayoutPaintCycler.hpp"
#include <chrono>

#include "../Helper.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/host/WindowHost.hpp"

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
  layout_dirty_ = true;
  paint_dirty_ = true;
}
}  // namespace cru::ui::host
