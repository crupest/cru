#include "cru/toml/TomlDocument.h"
#include "cru/toml/TomlParser.h"

#include <catch2/catch_test_macros.hpp>

using namespace cru::toml;

TEST_CASE("CruTomlParserTest Simple", "[toml]") {
  TomlParser parser(
      uR"(
a1 = v1
a2 = v2
# comment

[s1]
# comment
a3 = v3
a4 = v4

[s2]
a5 = v5
a6 = v6
  )");
  auto document = parser.Parse();
  REQUIRE(document.GetSection(u"")->GetValue(u"a1") == u"v1");
  REQUIRE(document.GetSection(u"")->GetValue(u"a2") == u"v2");
  REQUIRE(document.GetSection(u"s1")->GetValue(u"a3") == u"v3");
  REQUIRE(document.GetSection(u"s1")->GetValue(u"a4") == u"v4");
  REQUIRE(document.GetSection(u"s2")->GetValue(u"a5") == u"v5");
  REQUIRE(document.GetSection(u"s2")->GetValue(u"a6") == u"v6");
}
