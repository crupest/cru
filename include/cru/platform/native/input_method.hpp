#pragma once
#include "../resource.hpp"
#include "base.hpp"
#include "cru/common/event.hpp"

#include <memory>

namespace cru::platform::native {
// It is a reference, so there is a ref count, remember to destroy it to release
// the ref after use.
struct IInputMethodContextRef : INativeResource {
  // Reset composition string. Use this method to prepare typing.
  virtual void Reset() = 0;
  // Get the composition string.
  virtual std::string GetCompositionString() = 0;
  // Set the candidate window lefttop. Use this method to prepare typing.
  virtual void SetCandidateWindowPosition(const Point& point);
  // Triggered every time composition text changes, evet args is the new
  // composition text.
  virtual IEvent<std::string>* CompositionTextChangeEvent() = 0;
  // User comfirmed compostition. Use this method to get the final input text.
  virtual IEvent<std::string>* CharEvent() = 0;
};

struct IInputMethodManager : INativeResource {
  // Get a reference of context of a window.
  virtual std::unique_ptr<IInputMethodContextRef> GetContext(
      INativeWindow* window) = 0;
};
}  // namespace cru::platform::native
