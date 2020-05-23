#pragma once
#include "../../WinPreConfig.hpp"

#include "cru/common/Base.hpp"

namespace cru::platform::graph::win::direct {
template <typename TInterface>
struct IComResource : virtual Interface {
  virtual TInterface* GetComInterface() const = 0;
};
}  // namespace cru::platform::graph::win::direct
