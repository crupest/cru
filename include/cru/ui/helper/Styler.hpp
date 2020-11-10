#pragma once
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/helper/ClickDetector.hpp"
#include "gsl/pointers"

#include <memory>

namespace cru::ui::helper {
struct ControlStyleState {
  ClickState click_state;
  bool focus;
};

class Styler : public Object {
 public:
  // You could provide your click detector. Otherwise a new one will be created.
  explicit Styler(gsl::not_null<controls::Control*> control,
                  ClickDetector* click_detector = nullptr);

  CRU_DELETE_COPY(Styler)
  CRU_DELETE_MOVE(Styler)

  ~Styler();

 public:
  gsl::not_null<controls::Control*> GetControl() const { return control_; }
  gsl::not_null<ClickDetector*> GetClickDetector() const {
    return click_detector_;
  }

  IEvent<ControlStyleState>* StateChangeEvent() { return &state_change_event_; }

 private:
  void RaiseStateChangeEvent();

 private:
  gsl::not_null<controls::Control*> control_;
  std::unique_ptr<ClickDetector> managed_click_detector_;
  gsl::not_null<ClickDetector*> click_detector_;

  Event<ControlStyleState> state_change_event_;

  EventRevokerListGuard event_guard_;
};
}  // namespace cru::ui::helper
