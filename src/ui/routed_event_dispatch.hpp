#pragma once
#include "cru/ui/control.hpp"

#include "cru/common/logger.hpp"

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
void DispatchEvent(const std::wstring_view& event_name,
                   Control* const original_sender,
                   event::RoutedEvent<EventArgs>* (Control::*event_ptr)(),
                   Control* const last_receiver, Args&&... args) {
#ifdef CRU_DEBUG
  bool do_log = true;
  if (event_name == L"MouseMove") do_log = false;
#endif

  if (original_sender == last_receiver) {
    /*
    #ifdef CRU_DEBUG
        if (do_log)
          log::Debug(
              L"Routed event {} no need to dispatch (original_sender == "
              L"last_receiver). Original sender is {}.",
              event_name, original_sender->GetControlType());
    #endif
    */
    return;
  }

  std::list<Control*> receive_list;

  auto parent = original_sender;
  while (parent != last_receiver) {
    receive_list.push_back(parent);
    parent = parent->GetParent();
  }

#ifdef CRU_DEBUG
  if (do_log) {
    std::wstring log = L"Dispatch routed event ";
    log += event_name;
    log += L". Path (parent first): ";
    auto i = receive_list.crbegin();
    const auto end = --receive_list.crend();
    for (; i != end; ++i) {
      log += (*i)->GetControlType();
      log += L" -> ";
    }
    log += (*i)->GetControlType();
    log::Debug(log);
  }
#endif

  auto handled = false;

#ifdef CRU_DEBUG
  int count = 0;
#endif

  // tunnel
  for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
#ifdef CRU_DEBUG
    count++;
#endif
    EventArgs event_args(*i, original_sender, std::forward<Args>(args)...);
    static_cast<Event<EventArgs&>*>(((*i)->*event_ptr)()->Tunnel())
        ->Raise(event_args);
    if (event_args.IsHandled()) {
      handled = true;
#ifdef CRU_DEBUG
      if (do_log)
        log::Debug(
            L"Routed event is short-circuit in TUNNEL at {}-st control (count "
            L"from parent).",
            count);
#endif
      break;
    }
  }

  // bubble
  if (!handled) {
    for (auto i : receive_list) {
#ifdef CRU_DEBUG
      count--;
#endif
      EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
      static_cast<Event<EventArgs&>*>((i->*event_ptr)()->Bubble())
          ->Raise(event_args);
      if (event_args.IsHandled()) {
#ifdef CRU_DEBUG
        if (do_log)
          log::Debug(
              L"Routed event is short-circuit in BUBBLE at {}-st control "
              L"(count "
              L"from parent).",
              count);
#endif
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

#ifdef CRU_DEBUG
  if (do_log) log::Debug(L"Routed event dispatch finished.");
#endif
}
}  // namespace cru::ui
