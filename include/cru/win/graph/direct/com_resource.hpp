#pragma once
#include "../../win_pre_config.hpp"

namespace cru::platform::graph::win::direct {
template<typename TInterface>
struct IComResource {
  virtual ~IComResource() = default;

  virtual TInterface* GetComInterface() const = 0;
};
}  // namespace cru::platform::graph::win_direct
