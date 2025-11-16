#include "cru/ui/DeleteLater.h"

#include <catch2/catch_test_macros.hpp>

struct MockDeleteLater {
  bool triggered = false;

  void DeleteLater() { triggered = true; }
};

TEST_CASE("DeleteLaterPtr should work.", "[delete-later]") {
  auto ptr = cru::ui::MakeDeleteLater<MockDeleteLater>();
  auto p = ptr.get();
  ptr.reset();
  REQUIRE(p->triggered);
  delete p;
}
