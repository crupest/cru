#pragma once
#include "UiEventArgs.h"

#include <cru/base/Event.h>

namespace cru::ui::events {
// TEventArgs must not be a reference type. This class help add reference.
// EventArgs must be reference because the IsHandled property must be settable.
template <typename TEventArgs>
class CRU_UI_API RoutedEvent {
  friend controls::Window;

 public:
  static_assert(std::is_base_of_v<UiEventArgs, TEventArgs>,
                "TEventArgs must be subclass of UiEventArgs.");
  static_assert(!std::is_reference_v<TEventArgs>,
                "TEventArgs must not be reference.");

  using EventArgs = TEventArgs&;

  explicit RoutedEvent(std::string name) : name_(std::move(name)) {}

  std::string GetName() const { return name_; }

  IEvent<TEventArgs&>* Direct() { return &direct_; }
  IEvent<TEventArgs&>* Bubble() { return &bubble_; }
  IEvent<TEventArgs&>* Tunnel() { return &tunnel_; }

 private:
  std::string name_;

  Event<TEventArgs&> direct_;
  Event<TEventArgs&> bubble_;
  Event<TEventArgs&> tunnel_;
};

#define CRU_DEFINE_ROUTED_EVENT(name, arg_type)                 \
 private:                                                       \
  ::cru::ui::events::RoutedEvent<arg_type> name##Event_{#name}; \
                                                                \
 public:                                                        \
  ::cru::ui::events::RoutedEvent<arg_type>* name##Event() {     \
    return &name##Event_;                                       \
  }

}  // namespace cru::ui::events
