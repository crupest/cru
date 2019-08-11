#include "cru/ui/click_detector.hpp"

#include <cassert>
#include <optional>

namespace cru::ui {
ClickDetector::ClickDetector(Control* control) {
  assert(control);
  control_ = control;

  event_rovoker_guards_.push_front(
      EventRevokerGuard(control->MouseDownEvent()->Direct()->AddHandler(
          [this, control](event::MouseButtonEventArgs& args) {
            if (!control->CaptureMouse()) return;  // capture failed
            FromButton(args.GetMouseButton()) =
                args.GetPoint();  // save mouse down point
          })));

  event_rovoker_guards_.push_front(
      EventRevokerGuard(control->MouseUpEvent()->Direct()->AddHandler(
          [this, control](event::MouseButtonEventArgs& args) {
            if (!control->IsMouseCaptured()) return;
            const auto button = args.GetMouseButton();
            auto& down_point = FromButton(button);
            if (down_point.has_value()) {
              event_.Raise(ClickEventArgs(control, down_point.value(),
                                          args.GetPoint(), button));
              down_point = std::nullopt;
            }
          })));
}
}  // namespace cru::ui
