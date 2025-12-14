#include "cru/platform/GraphicsBase.h"

#include <catch2/catch_test_macros.hpp>

using cru::platform::Rect;

TEST_CASE("Rect IsIntersect", "[graphics][rect]") {
  auto test = [](const Rect& left, const Rect& right, bool intersect) {
    REQUIRE(left.IsIntersect(right) == intersect);
    REQUIRE(right.IsIntersect(left) == intersect);
  };

  test({}, {}, false);
  test({0.f, 0.f, 1.f, 1.f}, {}, false);

  test({0.f, 0.f, 1.f, 1.f}, {-2.f, 0.f, 1.f, 1.f}, false);
  test({0.f, 0.f, 1.f, 1.f}, {2.f, 0.25f, 1.f, 0.5f}, false);
  test({0.f, 0.f, 1.f, 1.f}, {0.f, -2.f, 0.5f, 1.f}, false);
  test({0.f, 0.f, 1.f, 1.f}, {0.f, 2.f, 0.5f, 1.f}, false);

  test({0.f, 0.f, 1.f, 1.f}, {-1.f, -1.f, 3.f, 0.5f}, false);
  test({0.f, 0.f, 1.f, 1.f}, {-1.f, -1.f, 3.f, 0.5f}, false);
  test({0.f, 0.f, 1.f, 1.f}, {-1.f, -1.f, 0.5f, 3.f}, false);
  test({0.f, 0.f, 1.f, 1.f}, {1.5f, -1.f, 0.5f, 3.f}, false);

  test({0.f, 0.f, 1.f, 1.f}, {-0.5f, 0.25f, 1.f, 0.5f}, true);
  test({0.f, 0.f, 1.f, 1.f}, {0.5f, 0.25f, 1.f, 0.5f}, true);
  test({0.f, 0.f, 1.f, 1.f}, {0.25f, -0.5f, 0.5f, 1.f}, true);
  test({0.f, 0.f, 1.f, 1.f}, {0.25f, 0.5f, 0.5f, 1.f}, true);

  test({0.f, 0.f, 1.f, 1.f}, {-0.5f, -0.5f, 1.f, 1.f}, true);
  test({0.f, 0.f, 1.f, 1.f}, {0.5f, -0.5f, 1.f, 1.f}, true);
  test({0.f, 0.f, 1.f, 1.f}, {0.5f, 0.5f, 1.f, 1.f}, true);
  test({0.f, 0.f, 1.f, 1.f}, {-0.5f, 0.5f, 1.f, 1.f}, true);
}

TEST_CASE("Rect Union", "[graphics][rect]") {
  auto test = [](const Rect& left, const Rect& right, const Rect& result) {
    REQUIRE(left.Union(right) == result);
    REQUIRE(right.Union(left) == result);
  };

  test({}, {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f, 1.f, 1.f});
  test({0.f, 0.f, 1.f, 1.f}, {2.f, 2.f, 1.f, 1.f}, {0.f, 0.f, 3.f, 3.f});
  test({2.f, 0.f, 1.f, 1.f}, {0.f, 2.f, 1.f, 1.f}, {0.f, 0.f, 3.f, 3.f});
  test({0.f, 0.f, 1.f, 1.f}, {0.5f, 0.5f, 1.f, 1.f}, {0.f, 0.f, 1.5f, 1.5f});
  test({0.5f, 0.f, 1.f, 1.f}, {0.f, 0.5f, 1.f, 1.f}, {0.f, 0.f, 1.5f, 1.5f});
}
