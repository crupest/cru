#pragma once
#include "UiEventArgs.h"

namespace cru::platform::graphics {
struct IPainter;
}

namespace cru::ui::events {
class CRU_UI_API PaintEventArgs : public UiEventArgs {
 public:
  PaintEventArgs(Object* sender, Object* original_sender,
                 platform::graphics::IPainter* painter)
      : UiEventArgs(sender, original_sender), painter_(painter) {}

  platform::graphics::IPainter* GetPainter() const { return painter_; }

 private:
  platform::graphics::IPainter* painter_;
};
}  // namespace cru::ui::event
