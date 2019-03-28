#pragma once
#include "pre.hpp"

#include <memory>

namespace cru::util {

template <typename TInterface>
std::shared_ptr<TInterface> CreateComSharedPtr(TInterface* p) {
  return std::shared_ptr<TInterface>(p, [](TInterface* ptr) {
    if (ptr != nullptr) ptr->Release();
  });
}

template <class TInterface>
void SafeRelease(TInterface*& p) {
  if (p != nullptr) {
    p->Release();
    p = nullptr;
  }
}
}  // namespace cru::util
