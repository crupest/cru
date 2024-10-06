#pragma once
#include "cru/base/Event.h"
#include "cru/ui/components/Component.h"

namespace cru::theme_builder::components {
class Editor : public ui::components::Component {
 public:
  Editor() = default;
  ~Editor() override = default;

 public:
  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }

 protected:
  void RaiseChangeEvent();
  void SuppressNextChangeEvent();
  void ConnectChangeEvent(IEvent<std::nullptr_t>* event);
  void ConnectChangeEvent(Editor* editor);
  void ConnectChangeEvent(Editor& editor) { ConnectChangeEvent(&editor); }

 private:
  bool suppress_next_change_event_ = false;
  Event<std::nullptr_t> change_event_;
};
}  // namespace cru::theme_builder::components
