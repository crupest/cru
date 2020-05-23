#include "cru/platform/native/UiApplication.hpp"

namespace cru::platform::native {
IUiApplication* IUiApplication::instance = nullptr;

IUiApplication::IUiApplication() {
  if (instance) {
    throw std::runtime_error("An ui application has already been created.");
  }
  
  instance = this;
}

IUiApplication::~IUiApplication() { instance = nullptr; }
}  // namespace cru::platform::native
