#include "cru/toml/TomlDocument.hpp"
#include "cru/toml/TomlParser.hpp"

#include <gtest/gtest.h>

using namespace cru::toml;

TEST(CruTomlParserTest, Simple) {
  TomlParser parser(
      uR"(
a1 = v1
"a2" = "v2"
# comment

[s1]
# comment
a3 = v3
"a4" = "v4"

[s2]
a5 = v5
"a6" = "v6"
  )");
  auto document = parser.Parse();
  ASSERT_EQ(document.GetSection(u"")->GetValue(u"a1"), u"v1");
  ASSERT_EQ(document.GetSection(u"")->GetValue(u"a2"), u"v2");
  ASSERT_EQ(document.GetSection(u"s1")->GetValue(u"a3"), u"v3");
  ASSERT_EQ(document.GetSection(u"s1")->GetValue(u"a4"), u"v4");
  ASSERT_EQ(document.GetSection(u"s2")->GetValue(u"a5"), u"v5");
  ASSERT_EQ(document.GetSection(u"s2")->GetValue(u"a6"), u"v6");
}
