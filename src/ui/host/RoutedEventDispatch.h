#pragma once
#include "cru/common/Logger.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/controls/Control.h"

#include <vector>

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
void DispatchEvent(
    const std::u16string_view& event_name,
    controls::Control* const original_sender,
    events::RoutedEvent<EventArgs>* (controls::Control::*event_ptr)(),
    controls::Control* const last_receiver, Args&&... args) {
  CRU_UNUSED(event_name)

  if (original_sender == last_receiver) {
    if constexpr (debug_flags::routed_event)
      log::Debug(
          "Routed event {} no need to dispatch (original_sender == "
          "last_receiver). Original sender is {}.",
          event_name, original_sender->GetControlType());
    return;
  }

  std::vector<controls::Control*> receive_list;

  auto parent = original_sender;
  while (parent != last_receiver) {
    receive_list.push_back(parent);
    parent = parent->GetParent();
  }

  if constexpr (debug_flags::routed_event) {
    std::u16string log = u"Dispatch routed event ";
    log += event_name;
    log += u". Path (parent first): ";
    auto i = receive_list.crbegin();
    const auto end = --receive_list.crend();
    for (; i != end; ++i) {
      log += (*i)->GetControlType();
      log += u" -> ";
    }
    log += (*i)->GetControlType();
    log::Debug(log);
  }

  auto handled = false;

  int count = 0;

  // tunnel
  for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
    count++;
    EventArgs event_args(*i, original_sender, std::forward<Args>(args)...);
    static_cast<Event<EventArgs&>*>(((*i)->*event_ptr)()->Tunnel())
        ->Raise(event_args);
    if (event_args.IsHandled()) {
      handled = true;
      if constexpr (debug_flags::routed_event)
        log::Debug(
            u"Routed event is short-circuit in TUNNEL at {}-st control (count "
            u"from parent).",
            count);
      break;
    }
  }

  // bubble
  if (!handled) {
    for (auto i : receive_list) {
      count--;
      EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
      static_cast<Event<EventArgs&>*>((i->*event_ptr)()->Bubble())
          ->Raise(event_args);
      if (event_args.IsHandled()) {
        if constexpr (debug_flags::routed_event)
          log::Debug(
              u"Routed event is short-circuit in BUBBLE at {}-st control "
              u"(count from parent).",
              count);
        break;
      }
    }
  }

  // direct
  for (auto i : receive_list) {
    EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
    static_cast<Event<EventArgs&>*>((i->*event_ptr)()->Direct())
        ->Raise(event_args);
  }

  if constexpr (debug_flags::routed_event)
    log::Debug(u"Routed event dispatch finished.");
}
}  // namespace cru::ui
