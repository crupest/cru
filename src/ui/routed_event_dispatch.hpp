#pragma once
#include "cru/ui/control.hpp"

#include <list>

namespace cru::ui {
// Dispatch the event.
//
// This will raise routed event of the control and its parent and parent's
// parent ... (until "last_receiver" if it's not nullptr) with appropriate args.
//
// First tunnel from top to bottom possibly stopped by "handled" flag in
// EventArgs. Second bubble from bottom to top possibly stopped by "handled"
// flag in EventArgs. Last direct to each control.
//
// Args is of type "EventArgs". The first init argument is "sender", which is
// automatically bound to each receiving control. The second init argument is
// "original_sender", which is unchanged. And "args" will be perfectly forwarded
// as the rest arguments.
template <typename EventArgs, typename... Args>
void DispatchEvent(Control* const original_sender,
                   event::RoutedEvent<EventArgs>* (Control::*event_ptr)(),
                   Control* const last_receiver, Args&&... args) {
  std::list<Control*> receive_list;

  auto parent = original_sender;
  while (parent != last_receiver) {
    receive_list.push_back(parent);
    parent = parent->GetParent();
  }

  auto handled = false;

  // tunnel
  for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
    EventArgs event_args(*i, original_sender, std::forward<Args>(args)...);
    static_cast<Event<EventArgs&>*>(((*i)->*event_ptr)()->Tunnel())
        ->Raise(event_args);
    if (event_args.IsHandled()) {
      handled = true;
      break;
    }
  }

  // bubble
  if (!handled) {
    for (auto i : receive_list) {
      EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
      static_cast<Event<EventArgs&>*>((i->*event_ptr)()->Bubble())
          ->Raise(event_args);
      if (event_args.IsHandled()) break;
    }
  }

  // direct
  for (auto i : receive_list) {
    EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
    static_cast<Event<EventArgs&>*>((i->*event_ptr)()->Direct())
        ->Raise(event_args);
  }
}
}  // namespace cru::ui
