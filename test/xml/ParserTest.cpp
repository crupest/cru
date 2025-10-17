#include "cru/xml/XmlNode.h"
#include "cru/xml/XmlParser.h"

#include <catch2/catch_test_macros.hpp>

using namespace cru::xml;

TEST_CASE("CruXmlParserTest Simple", "[xml]") {
  XmlParser parser("<root></root>");
  auto n = parser.Parse();
  REQUIRE(n->GetTag() == "root");
  REQUIRE(n->GetAttributes().empty() == true);
  REQUIRE(n->GetChildCount() == 0);
  delete n;
}

TEST_CASE("CruXmlParserTest SimpleWithAttribute", "[xml]") {
  XmlParser parser("<root a1=\"v1\" a2=\"v2\"></root>");
  auto n = parser.Parse();
  REQUIRE(n->GetTag() == "root");
  REQUIRE(n->GetAttributeValue("a1") == "v1");
  REQUIRE(n->GetAttributeValue("a2") == "v2");
  REQUIRE(n->GetChildCount() == 0);
  delete n;
}

TEST_CASE("CruXmlParserTest SimpleSelfClosing", "[xml]") {
  XmlParser parser("<root a1=\"v1\" a2=\"v2\"/>");
  auto n = parser.Parse();
  REQUIRE(n->GetTag() == "root");
  REQUIRE(n->GetAttributeValue("a1") == "v1");
  REQUIRE(n->GetAttributeValue("a2") == "v2");
  REQUIRE(n->GetChildCount() == 0);
  delete n;
}

TEST_CASE("CruXmlParserTest NestedElement", "[xml]") {
  XmlParser parser(
      "<root><c1><d1></d1></c1><c2><d2></d2><d3></d3></c2></root>");
  auto n = parser.Parse();
  REQUIRE(n->GetChildren().size() == 2);
  REQUIRE(n->GetChildAt(0)->AsElement()->GetTag() == "c1");
  REQUIRE(n->GetChildAt(1)->AsElement()->GetTag() == "c2");
  REQUIRE(n->GetChildAt(0)->AsElement()->GetChildCount() == 1);
  REQUIRE(n->GetChildAt(0)->AsElement()->GetChildAt(0)->AsElement()->GetTag() ==
          "d1");
  REQUIRE(n->GetChildAt(1)->AsElement()->GetChildCount() == 2);
  REQUIRE(n->GetChildAt(1)->AsElement()->GetChildAt(0)->AsElement()->GetTag() ==
          "d2");
  REQUIRE(n->GetChildAt(1)->AsElement()->GetChildAt(1)->AsElement()->GetTag() ==
          "d3");
  delete n;
}

TEST_CASE("CruXmlParserTest SimpleText", "[xml]") {
  XmlParser parser("<root>text</root>");
  auto n = parser.Parse();
  REQUIRE(n->GetChildCount() == 1);
  REQUIRE(n->GetChildAt(0)->AsText()->GetText() == "text");
  delete n;
}

TEST_CASE("CruXmlParserTest Whitespace", "[xml]") {
  XmlParser parser("\t\t<root>\n\t\t\ttext test\n\t\t</root>\t\t");
  auto n = parser.Parse();
  REQUIRE(n->GetChildCount() == 1);
  REQUIRE(n->GetChildAt(0)->AsText()->GetText() == "text test");
  delete n;
}

TEST_CASE("CruXmlParserTest Comment", "[xml]") {
  XmlParser parser("<!-- comment -->");
  auto n = parser.Parse();
  REQUIRE(n->IsCommentNode());
  REQUIRE(n->AsComment()->GetText() == "comment");
  delete n;
}

TEST_CASE("CruXmlParserTest Complex", "[xml]") {
  XmlParser parser(
      R"(
<root a1="v1">
  <c1>
    <d1>
    </d1>
  </c1>
  <c2 a2="v2" a3="v3">
    t1
    <d2 a4="v4"> t2 </d2>
    text test
    <d3></d3>
    t2
  </c2>
  <!-- comment -->
</root>
  )");
  auto n = parser.Parse();
  REQUIRE(n->GetAttributeValue("a1") == "v1");
  REQUIRE(n->GetChildCount() == 3);
  REQUIRE(n->GetChildAt(0)->AsElement()->GetTag() == "c1");
  REQUIRE(n->GetChildAt(0)->AsElement()->GetChildCount() == 1);
  auto c2 = n->GetChildAt(1)->AsElement();
  REQUIRE(c2->GetTag() == "c2");
  REQUIRE(c2->GetAttributeValue("a2") == "v2");
  REQUIRE(c2->GetAttributeValue("a3") == "v3");
  REQUIRE(c2->GetChildAt(0)->AsText()->GetText() == "t1");
  auto d2 = c2->GetChildAt(1)->AsElement();
  REQUIRE(d2->GetTag() == "d2");
  REQUIRE(d2->GetAttributeValue("a4") == "v4");
  REQUIRE(c2->GetChildAt(2)->AsText()->GetText() == "text test");
  REQUIRE(c2->GetChildAt(3)->AsElement()->GetTag() == "d3");
  REQUIRE(c2->GetChildAt(4)->AsText()->GetText() == "t2");
  REQUIRE(n->GetChildAt(2)->IsCommentNode());
  REQUIRE(n->GetChildAt(2)->AsComment()->GetText() == "comment");
  delete n;
}
