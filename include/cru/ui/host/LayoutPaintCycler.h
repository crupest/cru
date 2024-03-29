#pragma once
#include "../Base.h"

#include "cru/platform/gui/TimerHelper.h"
#include "cru/platform/gui/UiApplication.h"

#include <chrono>

namespace cru::ui::host {
class CRU_UI_API LayoutPaintCycler {
 public:
  explicit LayoutPaintCycler(WindowHost* host);

  CRU_DELETE_COPY(LayoutPaintCycler)
  CRU_DELETE_MOVE(LayoutPaintCycler)

  ~LayoutPaintCycler();

 public:
  void InvalidateLayout();
  void InvalidatePaint();

  bool IsLayoutDirty() { return layout_dirty_; }

 private:
  void OnCycle();

 private:
  WindowHost* host_;

  platform::gui::TimerAutoCanceler timer_canceler_;

  bool layout_dirty_ = true;
  bool paint_dirty_ = true;

  std::chrono::steady_clock::time_point last_cycle_time_;
  std::chrono::steady_clock::duration cycle_threshold_ =
      std::chrono::milliseconds(1000) / 144;
};
}  // namespace cru::ui::host
