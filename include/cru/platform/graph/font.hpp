#pragma once
#include "../native_resource.hpp"

namespace cru::platform::graph {
class Font : public NativeResource {
 protected:
  Font() = default;

 public:
  Font(const Font& other) = delete;
  Font& operator=(const Font& other) = delete;

  Font(Font&& other) = delete;
  Font& operator=(Font&& other) = delete;

  ~Font() override = default;
};
}  // namespace cru::platform::graph
