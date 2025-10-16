#include "cru/base/PropertyTree.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("PropertyTree", "[property_tree]") {
  using cru::PropertySubTreeRef;
  using cru::PropertyTree;

  PropertyTree tree({
      {"k1", "v1"},
      {"k2", "v2"},
      {"k3.sub", "v3"},
  });

  REQUIRE(tree.GetValue("k1") == "v1");
  REQUIRE(tree.GetValue("k2") == "v2");
  REQUIRE(tree.GetValue("k3.sub") == "v3");

  PropertySubTreeRef sub_tree = tree.GetSubTreeRef("k3");
  REQUIRE(sub_tree.GetValue("sub") == "v3");

  PropertySubTreeRef root_tree = sub_tree.GetParent();
  REQUIRE(root_tree.GetValue("k1") == "v1");
  REQUIRE(root_tree.GetValue("k2") == "v2");
  REQUIRE(root_tree.GetValue("k3.sub") == "v3");
}
