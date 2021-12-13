#pragma once
#include "UiEventArgs.hpp"

#include "cru/common/Event.hpp"

namespace cru::ui::events {
// TEventArgs must not be a reference type. This class help add reference.
// EventArgs must be reference because the IsHandled property must be settable.
template <typename TEventArgs>
class RoutedEvent {
 public:
  static_assert(std::is_base_of_v<UiEventArgs, TEventArgs>,
                "TEventArgs must be subclass of UiEventArgs.");
  static_assert(!std::is_reference_v<TEventArgs>,
                "TEventArgs must not be reference.");

  using RawEventArgs = TEventArgs;
  using IEventType = IEvent<TEventArgs&>;
  using EventArgs = typename IEventType::EventArgs;

  RoutedEvent() = default;
  RoutedEvent(const RoutedEvent& other) = delete;
  RoutedEvent(RoutedEvent&& other) = delete;
  RoutedEvent& operator=(const RoutedEvent& other) = delete;
  RoutedEvent& operator=(RoutedEvent&& other) = delete;
  ~RoutedEvent() = default;

  IEvent<TEventArgs&>* Direct() { return &direct_; }

  IEvent<TEventArgs&>* Bubble() { return &bubble_; }

  IEvent<TEventArgs&>* Tunnel() { return &tunnel_; }

 private:
  Event<TEventArgs&> direct_;
  Event<TEventArgs&> bubble_;
  Event<TEventArgs&> tunnel_;
};
}  // namespace cru::ui::event
