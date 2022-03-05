#include "cru/xml/XmlNode.h"
#include "cru/xml/XmlParser.h"

#include <gtest/gtest.h>

using namespace cru::xml;

TEST(CruXmlParserTest, Simple) {
  XmlParser parser(u"<root></root>");
  auto n = parser.Parse();
  ASSERT_EQ(n->GetTag(), u"root");
  ASSERT_EQ(n->GetAttributes().empty(), true);
  ASSERT_EQ(n->GetChildCount(), 0);
  delete n;
}

TEST(CruXmlParserTest, SimpleWithAttribute) {
  XmlParser parser(u"<root a1=\"v1\" a2=\"v2\"></root>");
  auto n = parser.Parse();
  ASSERT_EQ(n->GetTag(), u"root");
  ASSERT_EQ(n->GetAttributeValue(u"a1"), u"v1");
  ASSERT_EQ(n->GetAttributeValue(u"a2"), u"v2");
  ASSERT_EQ(n->GetChildCount(), 0);
  delete n;
}

TEST(CruXmlParserTest, SimpleSelfClosing) {
  XmlParser parser(u"<root a1=\"v1\" a2=\"v2\"/>");
  auto n = parser.Parse();
  ASSERT_EQ(n->GetTag(), u"root");
  ASSERT_EQ(n->GetAttributeValue(u"a1"), u"v1");
  ASSERT_EQ(n->GetAttributeValue(u"a2"), u"v2");
  ASSERT_EQ(n->GetChildCount(), 0);
  delete n;
}

TEST(CruXmlParserTest, NestedElement) {
  XmlParser parser(
      u"<root><c1><d1></d1></c1><c2><d2></d2><d3></d3></c2></root>");
  auto n = parser.Parse();
  ASSERT_EQ(n->GetChildren().size(), 2);
  ASSERT_EQ(n->GetChildAt(0)->AsElement()->GetTag(), u"c1");
  ASSERT_EQ(n->GetChildAt(1)->AsElement()->GetTag(), u"c2");
  ASSERT_EQ(n->GetChildAt(0)->AsElement()->GetChildCount(), 1);
  ASSERT_EQ(n->GetChildAt(0)->AsElement()->GetChildAt(0)->AsElement()->GetTag(),
            u"d1");
  ASSERT_EQ(n->GetChildAt(1)->AsElement()->GetChildCount(), 2);
  ASSERT_EQ(n->GetChildAt(1)->AsElement()->GetChildAt(0)->AsElement()->GetTag(),
            u"d2");
  ASSERT_EQ(n->GetChildAt(1)->AsElement()->GetChildAt(1)->AsElement()->GetTag(),
            u"d3");
  delete n;
}

TEST(CruXmlParserTest, SimpleText) {
  XmlParser parser(u"<root>text</root>");
  auto n = parser.Parse();
  ASSERT_EQ(n->GetChildCount(), 1);
  ASSERT_EQ(n->GetChildAt(0)->AsText()->GetText(), u"text");
  delete n;
}

TEST(CruXmlParserTest, Whitespace) {
  XmlParser parser(u"\t\t<root>\n\t\t\ttext test\n\t\t</root>\t\t");
  auto n = parser.Parse();
  ASSERT_EQ(n->GetChildCount(), 1);
  ASSERT_EQ(n->GetChildAt(0)->AsText()->GetText(), u"text test");
  delete n;
}

TEST(CruXmlParserTest, Comment) {
  XmlParser parser(u"<!-- comment -->");
  auto n = parser.Parse();
  ASSERT_TRUE(n->IsCommentNode());
  ASSERT_EQ(n->AsComment()->GetText(), u"comment");
  delete n;
}

TEST(CruXmlParserTest, Complex) {
  XmlParser parser(
      uR"(
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
  ASSERT_EQ(n->GetAttributeValue(u"a1"), u"v1");
  ASSERT_EQ(n->GetChildCount(), 3);
  ASSERT_EQ(n->GetChildAt(0)->AsElement()->GetTag(), u"c1");
  ASSERT_EQ(n->GetChildAt(0)->AsElement()->GetChildCount(), 1);
  auto c2 = n->GetChildAt(1)->AsElement();
  ASSERT_EQ(c2->GetTag(), u"c2");
  ASSERT_EQ(c2->GetAttributeValue(u"a2"), u"v2");
  ASSERT_EQ(c2->GetAttributeValue(u"a3"), u"v3");
  ASSERT_EQ(c2->GetChildAt(0)->AsText()->GetText(), u"t1");
  auto d2 = c2->GetChildAt(1)->AsElement();
  ASSERT_EQ(d2->GetTag(), u"d2");
  ASSERT_EQ(d2->GetAttributeValue(u"a4"), u"v4");
  ASSERT_EQ(c2->GetChildAt(2)->AsText()->GetText(), u"text test");
  ASSERT_EQ(c2->GetChildAt(3)->AsElement()->GetTag(), u"d3");
  ASSERT_EQ(c2->GetChildAt(4)->AsText()->GetText(), u"t2");
  ASSERT_TRUE(n->GetChildAt(2)->IsCommentNode());
  ASSERT_EQ(n->GetChildAt(2)->AsComment()->GetText(), u"comment");
  delete n;
}
