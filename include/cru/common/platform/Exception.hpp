#pragma once
#include "../Base.hpp"
#include "../Exception.hpp"

namespace cru::platform {
class CRU_BASE_API PlatformException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_COPY(PlatformException)
  CRU_DEFAULT_MOVE(PlatformException)

  CRU_DEFAULT_DESTRUCTOR(PlatformException)
};
}  // namespace cru::platform
