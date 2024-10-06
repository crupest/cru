#include "cru/base/PropertyTree.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("PropertyTree", "[property_tree]") {
  using cru::PropertySubTreeRef;
  using cru::PropertyTree;

  PropertyTree tree({
      {u"k1", u"v1"},
      {u"k2", u"v2"},
      {u"k3.sub", u"v3"},
  });

  REQUIRE(tree.GetValue(u"k1") == u"v1");
  REQUIRE(tree.GetValue(u"k2") == u"v2");
  REQUIRE(tree.GetValue(u"k3.sub") == u"v3");

  PropertySubTreeRef sub_tree = tree.GetSubTreeRef(u"k3");
  REQUIRE(sub_tree.GetValue(u"sub") == u"v3");

  PropertySubTreeRef root_tree = sub_tree.GetParent();
  REQUIRE(root_tree.GetValue(u"k1") == u"v1");
  REQUIRE(root_tree.GetValue(u"k2") == u"v2");
  REQUIRE(root_tree.GetValue(u"k3.sub") == u"v3");
}
