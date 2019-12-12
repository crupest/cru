#pragma once
#include "../../win_pre_config.hpp"

#include "cru/common/base.hpp"

namespace cru::platform::graph::win::direct {
template <typename TInterface>
struct IComResource : virtual Interface {
  virtual TInterface* GetComInterface() const = 0;
};
}  // namespace cru::platform::graph::win::direct
