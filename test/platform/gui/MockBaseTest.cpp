#include "cru/platform/gui/mock/Base.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using cru::platform::gui::mock::GetMockPlatformId;
using cru::platform::gui::mock::MockResource;

TEST_CASE("Mock GUI platform id is compile visible", "[platform][gui][mock]") {
  const MockResource resource;

  REQUIRE(std::string{MockResource::kPlatformId} == "Mock");
  REQUIRE(resource.GetPlatformId() == "Mock");
  REQUIRE(GetMockPlatformId() == "Mock");
}
