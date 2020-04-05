#pragma once
#include "../resource.hpp"
#include "base.hpp"
#include "cru/common/event.hpp"

#include <memory>

namespace cru::platform::native {
// It is a reference, so there is a ref count, remember to destroy it to release
// the ref after use.
struct IInputMethodContextRef : virtual INativeResource {
  // Return true if you should draw composition text manually. Return false if
  // system will take care of that for you.
  virtual bool ShouldManuallyDrawCompositionText() = 0;
  // Reset composition string. Use this method to prepare typing.
  virtual void Reset() = 0;
  // Get the composition string.
  virtual std::string GetCompositionText() = 0;
  // Set the candidate window lefttop. Use this method to prepare typing.
  virtual void SetCandidateWindowPosition(const Point& point) = 0;
  // Triggered when user starts composition.
  virtual IEvent<std::nullptr_t>* CompositionStartEvent() = 0;
  // Triggered when user stops composition.
  virtual IEvent<std::nullptr_t>* CompositionEndEvent() = 0;
  // Triggered every time composition text changes, event args is the new
  // composition text.
  virtual IEvent<std::string>* CompositionTextChangeEvent() = 0;
};

struct IInputMethodManager : virtual INativeResource {
  // Get a reference of context of a window.
  virtual std::unique_ptr<IInputMethodContextRef> GetContext(
      INativeWindow* window) = 0;
};
}  // namespace cru::platform::native
