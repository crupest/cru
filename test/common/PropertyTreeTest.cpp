#include "cru/common/PropertyTree.hpp"

#include <gtest/gtest.h>

TEST(PropertyTree, Test) {
  using cru::PropertySubTreeRef;
  using cru::PropertyTree;

  PropertyTree tree({
      {u"k1", u"v1"},
      {u"k2", u"v2"},
      {u"k3.sub", u"v3"},
  });

  ASSERT_EQ(tree.GetValue(u"k1"), u"v1");
  ASSERT_EQ(tree.GetValue(u"k2"), u"v2");
  ASSERT_EQ(tree.GetValue(u"k3.sub"), u"v3");

  PropertySubTreeRef sub_tree = tree.GetSubTreeRef(u"k3");
  ASSERT_EQ(sub_tree.GetValue(u"sub"), u"v3");

  PropertySubTreeRef root_tree = sub_tree.GetParent();
  ASSERT_EQ(root_tree.GetValue(u"k1"), u"v1");
  ASSERT_EQ(root_tree.GetValue(u"k2"), u"v2");
  ASSERT_EQ(root_tree.GetValue(u"k3.sub"), u"v3");
}
