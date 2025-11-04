#include "cru/base/toml/TomlDocument.h"
#include "cru/base/toml/TomlParser.h"

#include <catch2/catch_test_macros.hpp>

using namespace cru::toml;

TEST_CASE("CruTomlParserTest Simple", "[toml]") {
  TomlParser parser(
      R"(
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
  REQUIRE(document.GetSection("")->GetValue("a1") == "v1");
  REQUIRE(document.GetSection("")->GetValue("a2") == "v2");
  REQUIRE(document.GetSection("s1")->GetValue("a3") == "v3");
  REQUIRE(document.GetSection("s1")->GetValue("a4") == "v4");
  REQUIRE(document.GetSection("s2")->GetValue("a5") == "v5");
  REQUIRE(document.GetSection("s2")->GetValue("a6") == "v6");
}
