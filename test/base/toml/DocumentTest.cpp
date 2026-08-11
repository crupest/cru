#include "cru/base/toml/TomlDocument.h"

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace cru::toml;

namespace {
static_assert(std::same_as<TomlSection::value_type,
                           std::pair<std::string, cru::json::JsonValue>>);
static_assert(
    std::same_as<TomlSection::storage_type,
                 std::vector<std::pair<std::string, cru::json::JsonValue>>>);
static_assert(std::same_as<TomlSection::size_type,
                           typename TomlSection::storage_type::size_type>);
static_assert(
    std::same_as<TomlSection::difference_type,
                 typename TomlSection::storage_type::difference_type>);
static_assert(std::same_as<TomlSection::reference,
                           typename TomlSection::storage_type::reference>);
static_assert(
    std::same_as<TomlSection::const_reference,
                 typename TomlSection::storage_type::const_reference>);
static_assert(std::same_as<TomlSection::pointer,
                           typename TomlSection::storage_type::pointer>);
static_assert(std::same_as<TomlSection::const_pointer,
                           typename TomlSection::storage_type::const_pointer>);
static_assert(std::same_as<TomlSection::iterator,
                           typename TomlSection::storage_type::iterator>);
static_assert(std::same_as<TomlSection::const_iterator,
                           typename TomlSection::storage_type::const_iterator>);
static_assert(std::random_access_iterator<TomlSection::iterator>);
static_assert(std::random_access_iterator<TomlSection::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlSection&>().begin()),
                           TomlSection::iterator>);
static_assert(std::same_as<decltype(std::declval<const TomlSection&>().begin()),
                           TomlSection::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlSection&>().cbegin()),
                           TomlSection::const_iterator>);

static_assert(std::same_as<TomlDocument::value_type,
                           std::pair<std::string, TomlSection>>);
static_assert(std::same_as<TomlDocument::storage_type,
                           std::vector<std::pair<std::string, TomlSection>>>);
static_assert(std::same_as<TomlDocument::size_type,
                           typename TomlDocument::storage_type::size_type>);
static_assert(
    std::same_as<TomlDocument::difference_type,
                 typename TomlDocument::storage_type::difference_type>);
static_assert(std::same_as<TomlDocument::reference,
                           typename TomlDocument::storage_type::reference>);
static_assert(
    std::same_as<TomlDocument::const_reference,
                 typename TomlDocument::storage_type::const_reference>);
static_assert(std::same_as<TomlDocument::pointer,
                           typename TomlDocument::storage_type::pointer>);
static_assert(std::same_as<TomlDocument::const_pointer,
                           typename TomlDocument::storage_type::const_pointer>);
static_assert(std::same_as<TomlDocument::iterator,
                           typename TomlDocument::storage_type::iterator>);
static_assert(
    std::same_as<TomlDocument::const_iterator,
                 typename TomlDocument::storage_type::const_iterator>);
static_assert(std::random_access_iterator<TomlDocument::iterator>);
static_assert(std::random_access_iterator<TomlDocument::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlDocument&>().begin()),
                           TomlDocument::iterator>);
static_assert(
    std::same_as<decltype(std::declval<const TomlDocument&>().begin()),
                 TomlDocument::const_iterator>);
static_assert(std::same_as<decltype(std::declval<TomlDocument&>().cbegin()),
                           TomlDocument::const_iterator>);
}  // namespace

TEST_CASE("TomlSection supports value lookup mutation deletion and iteration",
          "[toml]") {
  TomlSection section;
  REQUIRE(section.begin() == section.end());
  REQUIRE_FALSE(section.HasKey("title"));

  section.SetValue("title", cru::json::JsonValue("demo"));
  section.SetValue("enabled", cru::json::JsonValue(true));
  REQUIRE(section.HasKey("title"));
  REQUIRE(section.GetValue("title") == "demo");
  REQUIRE(section.GetValue("enabled") == true);

  section.SetValue("title", cru::json::JsonValue("changed"));
  REQUIRE(section.GetValue("title") == "changed");

  TomlSection::iterator title = section.begin();
  REQUIRE(title->first == "title");
  REQUIRE(title->second == "changed");
  title->second = false;
  REQUIRE(section.GetValue("title") == false);

  const TomlSection& const_section = section;
  TomlSection::const_iterator enabled = const_section.begin() + 1;
  REQUIRE(enabled->first == "enabled");
  REQUIRE(enabled->second == true);
  REQUIRE(const_section.cbegin()->first == "title");
  REQUIRE(const_section.cend() - const_section.cbegin() == 2);

  std::vector<std::string> value_names;
  for (TomlSection::value_type& value : section) {
    value_names.push_back(value.first);
    if (value.first == "enabled") {
      value.second = false;
    }
  }
  REQUIRE(value_names == std::vector<std::string>{"title", "enabled"});
  REQUIRE(section.GetValue("enabled") == false);

  std::vector<std::string> const_value_names;
  for (const TomlSection::value_type& value : const_section) {
    const_value_names.push_back(value.first);
  }
  REQUIRE(const_value_names == value_names);

  REQUIRE_FALSE(section.DeleteValue("missing"));
  REQUIRE(section.DeleteValue("title"));
  REQUIRE_FALSE(section.HasKey("title"));
  REQUIRE(section.begin()->first == "enabled");
}

TEST_CASE(
    "TomlDocument supports section lookup mutation deletion and iteration",
    "[toml]") {
  TomlDocument document;
  REQUIRE(document.begin() == document.end());
  REQUIRE_FALSE(document.HasSection("root"));
  REQUIRE_THROWS_AS(document.GetSection("root"), cru::Exception);

  TomlSection root;
  root.SetValue("title", cru::json::JsonValue("demo"));
  document.SetSection("root", std::move(root));

  TomlSection& root_section = document.GetSection("root");
  REQUIRE(root_section.GetValue("title") == "demo");

  TomlSection& owner = document.GetSectionOrCreate("owner");
  owner.SetValue("name", cru::json::JsonValue("Ada"));
  REQUIRE(&document.GetSectionOrCreate("owner") == &owner);

  std::vector<std::string> section_names;
  for (TomlDocument::value_type& section : document) {
    section_names.push_back(section.first);
    if (section.first == "owner") {
      section.second.SetValue("active", cru::json::JsonValue(true));
    }
  }
  REQUIRE(section_names == std::vector<std::string>{"root", "owner"});
  REQUIRE(document.GetSection("owner").GetValue("active") == true);

  const TomlDocument& const_document = document;
  std::vector<std::string> const_section_names;
  for (const TomlDocument::value_type& section : const_document) {
    const_section_names.push_back(section.first);
  }
  REQUIRE(const_section_names == section_names);

  TomlDocument::const_iterator first_section = const_document.begin();
  REQUIRE(first_section->first == "root");
  REQUIRE(const_document.GetSection("owner").GetValue("name") == "Ada");
  REQUIRE(const_document.cend() - const_document.cbegin() == 2);
  REQUIRE(const_document.HasSection("owner"));
  REQUIRE_THROWS_AS(const_document.GetSection("missing"), cru::Exception);

  TomlSection replacement;
  replacement.SetValue("name", cru::json::JsonValue("Grace"));
  document.SetSection("owner", std::move(replacement));
  REQUIRE(document.GetSection("owner").GetValue("name") == "Grace");

  REQUIRE_FALSE(document.DeleteSection("missing"));
  REQUIRE(document.DeleteSection("root"));
  REQUIRE_FALSE(document.HasSection("root"));
  REQUIRE_THROWS_AS(document.GetSection("root"), cru::Exception);
  REQUIRE(document.begin()->first == "owner");
}
