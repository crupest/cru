#include "cru/base/datamodel/DataType.h"

#include <catch2/catch_test_macros.hpp>
#include <unordered_map>
#include <vector>

using cru::Exception;
using cru::datamodel::DataConvertResult;
using cru::datamodel::NumberDataType;
using cru::datamodel::StringDataType;
using cru::string::ParseToNumberFlags;
using cru::xml::XmlElementNode;
using cru::xml::XmlTextNode;

TEST_CASE("DataConvertResult basic behaviors", "[datamodel]") {
  auto success = DataConvertResult<int>::Success(42);
  REQUIRE(success.IsSuccess());
  REQUIRE(!success.HasErrors());
  REQUIRE(success.GetValue() == 42);
  REQUIRE(success.GetOptionalValue() == std::optional<int>(42));

  success.AddError("recoverable warning");
  REQUIRE(success.HasErrors());
  REQUIRE(success.GetErrors() ==
          std::vector<std::string>{"recoverable warning"});

  success.RemoveError("recoverable warning");
  REQUIRE(!success.HasErrors());

  auto failed = DataConvertResult<int>::Failure("not a number");
  REQUIRE(!failed.IsSuccess());
  REQUIRE(failed.HasErrors());
  REQUIRE(failed.GetErrors() == std::vector<std::string>{"not a number"});
  REQUIRE_THROWS_AS(failed.GetValue(), Exception);

  auto success_with_errors =
      DataConvertResult<int>::SuccessWithErrors(7, {"w1", "w2"});
  REQUIRE(success_with_errors.IsSuccess());
  REQUIRE(success_with_errors.HasErrors());
  REQUIRE(success_with_errors.GetValue() == 7);
  REQUIRE(success_with_errors.GetErrors() ==
          std::vector<std::string>{"w1", "w2"});
}

TEST_CASE("NumberDataType string conversion", "[datamodel]") {
  NumberDataType<int> type;

  REQUIRE(type.GetName() == "Number");
  REQUIRE(type.SupportConvertFromString());
  REQUIRE(type.SupportConvertToString());
  REQUIRE(type.SupportConvertFromXml());
  REQUIRE(!type.SupportConvertToXml());

  auto parsed = type.ConvertFromString(" 10 trailing");
  REQUIRE(parsed.IsSuccess());
  REQUIRE(parsed.GetValue() == 10);

  auto failed = type.ConvertFromString("abc");
  REQUIRE(!failed.IsSuccess());
  REQUIRE(failed.HasErrors());

  auto serialized = type.ConvertToString(-25);
  REQUIRE(serialized.IsSuccess());
  REQUIRE(serialized.GetValue() == "-25");

  REQUIRE_THROWS_AS(type.ConvertToXml(1), Exception);
}

TEST_CASE("NumberDataType strict trailing junk handling", "[datamodel]") {
  NumberDataType<int> strict_type(ParseToNumberFlags::AllowLeadingSpaces |
                                  ParseToNumberFlags::AllowTrailingSpaces);

  auto parsed = strict_type.ConvertFromString(" 55 ");
  REQUIRE(parsed.IsSuccess());
  REQUIRE(parsed.GetValue() == 55);

  auto failed = strict_type.ConvertFromString("55junk");
  REQUIRE(!failed.IsSuccess());
  REQUIRE(failed.HasErrors());
}

TEST_CASE("NumberDataType xml conversion through string fallback", "[datamodel]") {
  NumberDataType<int> type;

  XmlElementNode from_text("Number");
  from_text.AddChild(new XmlTextNode("77"));
  REQUIRE(type.XmlIsOfThisType(&from_text));

  auto parsed_from_text = type.ConvertFromXml(&from_text);
  REQUIRE(parsed_from_text.IsSuccess());
  REQUIRE(parsed_from_text.GetValue() == 77);

  XmlElementNode wrong_tag("String");
  wrong_tag.AddChild(new XmlTextNode("77"));
  REQUIRE(!type.XmlIsOfThisType(&wrong_tag));
  REQUIRE_THROWS_AS(type.ConvertFromXml(&wrong_tag), Exception);

  XmlElementNode nested_element("Number");
  nested_element.AddChild(new XmlElementNode("child"));

  auto nested_failed = type.ConvertFromXml(&nested_element);
  REQUIRE(!nested_failed.IsSuccess());
  REQUIRE(nested_failed.HasErrors());

  XmlElementNode attr_and_text(
      "Number", std::unordered_map<std::string, std::string>{{"value", "33"}});
  attr_and_text.AddChild(new XmlTextNode("44"));

  auto attr_and_text_result = type.ConvertFromXml(&attr_and_text);
  REQUIRE(attr_and_text_result.IsSuccess());
  REQUIRE(attr_and_text_result.GetValue() == 33);
  REQUIRE(attr_and_text_result.HasErrors());

  XmlElementNode attr_only(
      "Number", std::unordered_map<std::string, std::string>{{"value", "99"}});
  auto attr_only_result = type.ConvertFromXml(&attr_only);
  REQUIRE(attr_only_result.IsSuccess());
  REQUIRE(attr_only_result.GetValue() == 99);
  REQUIRE(!attr_only_result.HasErrors());
}

TEST_CASE("StringDataType conversion", "[datamodel]") {
  StringDataType type;

  REQUIRE(type.GetName() == "String");
  REQUIRE(type.SupportConvertFromString());
  REQUIRE(type.SupportConvertToString());
  REQUIRE(type.SupportConvertFromXml());
  REQUIRE(!type.SupportConvertToXml());

  auto parsed = type.ConvertFromString("hello");
  REQUIRE(parsed.IsSuccess());
  REQUIRE(parsed.GetValue() == "hello");

  auto serialized = type.ConvertToString("world");
  REQUIRE(serialized.IsSuccess());
  REQUIRE(serialized.GetValue() == "world");

  XmlElementNode xml_value("String");
  xml_value.AddChild(new XmlTextNode("from-xml"));

  auto from_xml = type.ConvertFromXml(&xml_value);
  REQUIRE(from_xml.IsSuccess());
  REQUIRE(from_xml.GetValue() == "from-xml");

  REQUIRE_THROWS_AS(type.ConvertToXml(std::string("x")), Exception);
}
