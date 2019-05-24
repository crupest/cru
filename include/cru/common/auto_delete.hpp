#pragma once
#include "base.hpp"

namespace cru {
// A instance of class implementing this interface is able to
// delete itseft when program exits. Such as IGraphFactory,
// IUiApplication.
struct IAutoDelete : virtual Interface {
  // Get whether it will delete itself when program exits.
  virtual bool IsAutoDelete() const = 0;
  // Set whether it will delete itself when program exits.
  virtual void SetAutoDelete(bool value) = 0;
};
}  // namespace cru
