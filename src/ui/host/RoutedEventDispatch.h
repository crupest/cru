#pragma once
#include "cru/base/SelfResolvable.h"
#include "cru/base/log/Logger.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/host/WindowHost.h"

#include <vector>

namespace cru::ui::host {
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
    const String& event_name, controls::Control* const original_sender,
    events::RoutedEvent<EventArgs>* (controls::Control::*event_ptr)(),
    controls::Control* const last_receiver, Args&&... args) {
  constexpr auto kLogTag = "DispatchEvent";

  if (original_sender == nullptr) return;

  CRU_UNUSED(event_name)

  if (original_sender == last_receiver) {
    if constexpr (debug_flags::routed_event)
      CRU_LOG_TAG_DEBUG(
          "Routed event {} no need to dispatch (original_sender == "
          "last_receiver). Original sender is {}.",
          event_name.ToUtf8(), original_sender->GetControlType().ToUtf8());
    return;
  }

  WindowHost::EnterEventHandling();

  std::vector<ObjectResolver<controls::Control>> receive_list;

  auto parent = original_sender;
  while (parent != last_receiver) {
    receive_list.push_back(parent->CreateResolver());
    auto p = parent->GetParent();
    assert(!(p == nullptr && last_receiver != nullptr));
    parent = p;
  }

  if constexpr (debug_flags::routed_event) {
    std::string log = "Dispatch routed event ";
    log += event_name.ToUtf8();
    log += ". Path (parent first): ";
    auto i = receive_list.crbegin();
    const auto end = --receive_list.crend();
    for (; i != end; ++i) {
      log += i->Resolve()->GetControlType().ToUtf8();
      log += " -> ";
    }
    log += i->Resolve()->GetControlType().ToUtf8();
    CRU_LOG_TAG_DEBUG(log);
  }

  auto handled = false;

  int count = 0;

  // tunnel
  for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i) {
    count++;
    auto control = i->Resolve();
    if (!control) continue;
    EventArgs event_args(control, original_sender, std::forward<Args>(args)...);
    static_cast<Event<EventArgs&>*>((control->*event_ptr)()->Tunnel())
        ->Raise(event_args);
    if (event_args.IsHandled()) {
      handled = true;
      if constexpr (debug_flags::routed_event)
        CRU_LOG_TAG_DEBUG(
            "Routed event is short-circuit in TUNNEL at {}-st control (count "
            "from parent).",
            count);
      break;
    }
  }

  // bubble
  if (!handled) {
    for (const auto& resolver : receive_list) {
      count--;
      auto control = resolver.Resolve();
      if (!control) continue;
      EventArgs event_args(control, original_sender,
                           std::forward<Args>(args)...);
      static_cast<Event<EventArgs&>*>((control->*event_ptr)()->Bubble())
          ->Raise(event_args);
      if (event_args.IsHandled()) {
        if constexpr (debug_flags::routed_event)
          CRU_LOG_TAG_DEBUG(
              "Routed event is short-circuit in BUBBLE at {}-st control "
              "(count from parent).",
              count);
        break;
      }
    }
  }

  // direct
  for (auto resolver : receive_list) {
    auto control = resolver.Resolve();
    if (!control) continue;
    EventArgs event_args(control, original_sender, std::forward<Args>(args)...);
    static_cast<Event<EventArgs&>*>((control->*event_ptr)()->Direct())
        ->Raise(event_args);
  }

  if constexpr (debug_flags::routed_event)
    CRU_LOG_TAG_DEBUG("Routed event dispatch finished.");

  WindowHost::LeaveEventHandling();
}
}  // namespace cru::ui::host
