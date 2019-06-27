#pragma once
#include "cru/common/base.hpp"

#include <string_view>

namespace cru::platform {
class NativeResource : public Object {
 protected:
  NativeResource() = default;

 public:
  NativeResource(const NativeResource& other) = delete;
  NativeResource& operator=(const NativeResource& other) = delete;
                

  NativeResource(NativeResource&& other) = delete;
  NativeResource& operator=(NativeResource&& other) = delete;
  ~NativeResource() override = default;

 public:
  virtual std::wstring_view GetPlatformId() const = 0;
};
}  // namespace cru::platform
