#include "cru/base/Dictionary.h"

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <iterator>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace {
using StringIntDictionary = cru::Dictionary<std::string, int>;

template <typename Action>
void RequireOutOfRangeContaining(Action action, std::string_view text) {
  try {
    action();
    FAIL("Expected Dictionary lookup to throw");
  } catch (const std::out_of_range& exception) {
    REQUIRE(std::string_view(exception.what()).find(text) !=
            std::string_view::npos);
  }
}

std::vector<std::string> Keys(const StringIntDictionary& dictionary) {
  std::vector<std::string> keys;
  for (const auto& item : dictionary) {
    keys.push_back(item.first);
  }
  return keys;
}

static_assert(std::same_as<StringIntDictionary::StorageType,
                           std::list<std::pair<const std::string, int>>>);
static_assert(std::same_as<StringIntDictionary::key_type, std::string>);
static_assert(std::same_as<StringIntDictionary::mapped_type, int>);
static_assert(std::same_as<StringIntDictionary::value_type,
                           StringIntDictionary::StorageType::value_type>);
static_assert(std::is_const_v<StringIntDictionary::value_type::first_type>);
static_assert(std::same_as<StringIntDictionary::size_type,
                           StringIntDictionary::StorageType::size_type>);
static_assert(std::same_as<StringIntDictionary::difference_type,
                           StringIntDictionary::StorageType::difference_type>);
static_assert(std::same_as<StringIntDictionary::allocator_type,
                           StringIntDictionary::StorageType::allocator_type>);
static_assert(std::same_as<StringIntDictionary::reference,
                           StringIntDictionary::StorageType::reference>);
static_assert(std::same_as<StringIntDictionary::const_reference,
                           StringIntDictionary::StorageType::const_reference>);
static_assert(std::same_as<StringIntDictionary::pointer,
                           StringIntDictionary::StorageType::pointer>);
static_assert(std::same_as<StringIntDictionary::const_pointer,
                           StringIntDictionary::StorageType::const_pointer>);
static_assert(std::same_as<StringIntDictionary::iterator,
                           StringIntDictionary::StorageType::iterator>);
static_assert(std::same_as<StringIntDictionary::const_iterator,
                           StringIntDictionary::StorageType::const_iterator>);
static_assert(std::same_as<StringIntDictionary::reverse_iterator,
                           StringIntDictionary::StorageType::reverse_iterator>);
static_assert(
    std::same_as<StringIntDictionary::const_reverse_iterator,
                 StringIntDictionary::StorageType::const_reverse_iterator>);
static_assert(std::bidirectional_iterator<StringIntDictionary::iterator>);
static_assert(std::bidirectional_iterator<StringIntDictionary::const_iterator>);
static_assert(
    std::bidirectional_iterator<StringIntDictionary::reverse_iterator>);
static_assert(
    std::bidirectional_iterator<StringIntDictionary::const_reverse_iterator>);
static_assert(!std::random_access_iterator<StringIntDictionary::iterator>);
static_assert(
    !std::random_access_iterator<StringIntDictionary::const_iterator>);
static_assert(
    !std::random_access_iterator<StringIntDictionary::reverse_iterator>);
static_assert(
    !std::random_access_iterator<StringIntDictionary::const_reverse_iterator>);
static_assert(
    std::same_as<decltype(std::declval<StringIntDictionary&>().begin()),
                 StringIntDictionary::iterator>);
static_assert(
    std::same_as<decltype(std::declval<const StringIntDictionary&>().begin()),
                 StringIntDictionary::const_iterator>);
static_assert(
    std::same_as<decltype(std::declval<StringIntDictionary&>().cbegin()),
                 StringIntDictionary::const_iterator>);
static_assert(
    std::same_as<decltype(std::declval<StringIntDictionary&>().rbegin()),
                 StringIntDictionary::reverse_iterator>);
static_assert(
    std::same_as<decltype(std::declval<StringIntDictionary&>().rend()),
                 StringIntDictionary::reverse_iterator>);
static_assert(
    std::same_as<decltype(std::declval<const StringIntDictionary&>().rbegin()),
                 StringIntDictionary::const_reverse_iterator>);
static_assert(
    std::same_as<decltype(std::declval<const StringIntDictionary&>().rend()),
                 StringIntDictionary::const_reverse_iterator>);
static_assert(
    std::same_as<decltype(std::declval<StringIntDictionary&>().crbegin()),
                 StringIntDictionary::const_reverse_iterator>);
static_assert(
    std::same_as<decltype(std::declval<StringIntDictionary&>().crend()),
                 StringIntDictionary::const_reverse_iterator>);
}  // namespace

TEST_CASE("Dictionary exposes list-like container metadata", "[dictionary]") {
  StringIntDictionary dictionary;

  REQUIRE(dictionary.empty());
  REQUIRE(dictionary.size() == 0);
  REQUIRE(dictionary.begin() == dictionary.end());
  REQUIRE(dictionary.cbegin() == dictionary.cend());
  REQUIRE(dictionary.max_size() > 0);

  dictionary.emplace("alpha", 1);
  dictionary.clear();
  REQUIRE(dictionary.empty());
}

TEST_CASE("Dictionary inserts unique keys and preserves iteration order",
          "[dictionary]") {
  StringIntDictionary dictionary;

  auto [alpha, inserted_alpha] =
      dictionary.insert(StringIntDictionary::value_type{"alpha", 1});
  REQUIRE(inserted_alpha);
  REQUIRE(alpha == dictionary.begin());
  REQUIRE(alpha->first == "alpha");
  REQUIRE(alpha->second == 1);

  auto [duplicate_alpha, inserted_duplicate_alpha] =
      dictionary.insert(StringIntDictionary::value_type{"alpha", 2});
  REQUIRE_FALSE(inserted_duplicate_alpha);
  REQUIRE(duplicate_alpha == dictionary.begin());
  REQUIRE(duplicate_alpha->second == 1);
  REQUIRE(dictionary.size() == 1);

  auto [beta, inserted_beta] = dictionary.emplace("beta", 2);
  REQUIRE(inserted_beta);
  REQUIRE(beta->first == "beta");
  REQUIRE(beta->second == 2);

  auto [gamma, inserted_gamma] = dictionary.emplace("gamma", 3);
  REQUIRE(inserted_gamma);
  REQUIRE(gamma->first == "gamma");
  REQUIRE(gamma->second == 3);

  REQUIRE(Keys(dictionary) ==
          std::vector<std::string>{"alpha", "beta", "gamma"});

  for (auto& item : dictionary) {
    if (item.first == "beta") {
      item.second = 20;
    }
  }
  REQUIRE(dictionary.at("beta") == 20);

  const auto& const_dictionary = dictionary;
  REQUIRE(const_dictionary.begin()->first == "alpha");
  REQUIRE(std::distance(const_dictionary.cbegin(), const_dictionary.cend()) ==
          3);
}

TEST_CASE("Dictionary iterates in reverse insertion order", "[dictionary]") {
  StringIntDictionary dictionary;
  dictionary.emplace("alpha", 1);
  dictionary.emplace("beta", 2);
  dictionary.emplace("gamma", 3);

  REQUIRE(dictionary.rbegin()->first == "gamma");
  REQUIRE(std::distance(dictionary.rbegin(), dictionary.rend()) == 3);
  dictionary.rbegin()->second = 30;
  REQUIRE(dictionary.at("gamma") == 30);

  std::vector<std::string> reverse_keys;
  for (auto it = dictionary.rbegin(); it != dictionary.rend(); ++it) {
    reverse_keys.push_back(it->first);
  }
  REQUIRE(reverse_keys == std::vector<std::string>{"gamma", "beta", "alpha"});

  const auto& const_dictionary = dictionary;
  std::vector<std::string> const_reverse_keys;
  for (auto it = const_dictionary.rbegin(); it != const_dictionary.rend();
       ++it) {
    const_reverse_keys.push_back(it->first);
  }
  REQUIRE(const_reverse_keys ==
          std::vector<std::string>{"gamma", "beta", "alpha"});

  std::vector<std::string> explicit_const_reverse_keys;
  for (auto it = dictionary.crbegin(); it != dictionary.crend(); ++it) {
    explicit_const_reverse_keys.push_back(it->first);
  }
  REQUIRE(explicit_const_reverse_keys ==
          std::vector<std::string>{"gamma", "beta", "alpha"});
}

TEST_CASE("Dictionary supports lookup mutation and default insertion",
          "[dictionary]") {
  StringIntDictionary dictionary;
  dictionary.emplace("alpha", 1);
  dictionary.emplace("beta", 2);

  REQUIRE(dictionary.contains("alpha"));
  REQUIRE_FALSE(dictionary.contains("missing"));
  REQUIRE(dictionary.count("beta") == 1);
  REQUIRE(dictionary.count("missing") == 0);

  auto alpha = dictionary.find("alpha");
  REQUIRE(alpha != dictionary.end());
  REQUIRE(alpha->second == 1);
  REQUIRE(dictionary.find("missing") == dictionary.end());

  const auto& const_dictionary = dictionary;
  auto const_beta = const_dictionary.find("beta");
  REQUIRE(const_beta != const_dictionary.end());
  REQUIRE(const_beta->second == 2);
  REQUIRE(const_dictionary.find("missing") == const_dictionary.end());

  dictionary.at("alpha") = 10;
  REQUIRE(const_dictionary.at("alpha") == 10);
  RequireOutOfRangeContaining([&] { dictionary.at("missing"); },
                              "Key not found");
  RequireOutOfRangeContaining([&] { const_dictionary.at("missing"); },
                              "Key not found");

  dictionary["beta"] = 20;
  REQUIRE(dictionary.size() == 2);
  REQUIRE(dictionary.at("beta") == 20);

  int& inserted_value = dictionary["delta"];
  REQUIRE(inserted_value == 0);
  inserted_value = 4;
  REQUIRE(dictionary.at("delta") == 4);
  REQUIRE(Keys(dictionary) ==
          std::vector<std::string>{"alpha", "beta", "delta"});
}

TEST_CASE("Dictionary insert_or_assign updates values without reordering",
          "[dictionary]") {
  StringIntDictionary dictionary;
  dictionary.emplace("alpha", 1);
  dictionary.emplace("gamma", 3);

  auto [beta, inserted_beta] = dictionary.insert_or_assign("beta", 2);
  REQUIRE(inserted_beta);
  REQUIRE(beta->first == "beta");
  REQUIRE(beta->second == 2);

  auto [alpha, inserted_alpha] = dictionary.insert_or_assign("alpha", 10);
  REQUIRE_FALSE(inserted_alpha);
  REQUIRE(alpha == dictionary.begin());
  REQUIRE(alpha->second == 10);
  REQUIRE(Keys(dictionary) ==
          std::vector<std::string>{"alpha", "gamma", "beta"});

  cru::Dictionary<std::string, std::unique_ptr<int>> move_only_dictionary;
  auto [first, inserted_first] =
      move_only_dictionary.insert_or_assign("first", std::make_unique<int>(1));
  REQUIRE(inserted_first);
  REQUIRE(*first->second == 1);

  auto replacement = std::make_unique<int>(2);
  auto [assigned_first, inserted_assigned_first] =
      move_only_dictionary.insert_or_assign(std::string("first"),
                                            std::move(replacement));
  REQUIRE_FALSE(inserted_assigned_first);
  REQUIRE(replacement == nullptr);
  REQUIRE(*assigned_first->second == 2);

  auto [second, inserted_second] =
      move_only_dictionary.emplace("second", std::make_unique<int>(3));
  REQUIRE(inserted_second);
  REQUIRE(*second->second == 3);
}

TEST_CASE("Dictionary erases by key iterator range and predicate",
          "[dictionary]") {
  StringIntDictionary dictionary;
  dictionary.emplace("alpha", 1);
  dictionary.emplace("beta", 2);
  dictionary.emplace("gamma", 3);
  dictionary.emplace("delta", 4);
  dictionary.emplace("epsilon", 5);

  auto gamma = dictionary.find("gamma");
  REQUIRE(gamma != dictionary.end());

  REQUIRE(dictionary.erase("missing") == 0);
  REQUIRE(dictionary.erase("beta") == 1);
  REQUIRE(gamma->first == "gamma");
  REQUIRE(gamma->second == 3);
  REQUIRE(Keys(dictionary) ==
          std::vector<std::string>{"alpha", "gamma", "delta", "epsilon"});

  auto after_first = dictionary.erase(dictionary.begin());
  REQUIRE(after_first == dictionary.begin());
  REQUIRE(after_first->first == "gamma");

  auto after_range =
      dictionary.erase(dictionary.find("delta"), dictionary.end());
  REQUIRE(after_range == dictionary.end());
  REQUIRE(Keys(dictionary) == std::vector<std::string>{"gamma"});

  dictionary.emplace("delta", 4);
  dictionary.emplace("epsilon", 5);
  auto erased_count = erase_if(
      dictionary, [](const auto& item) { return item.second % 2 != 0; });
  REQUIRE(erased_count == 2);
  REQUIRE(Keys(dictionary) == std::vector<std::string>{"delta"});
}

TEST_CASE("Dictionary compares and swaps stored ordered pairs",
          "[dictionary]") {
  StringIntDictionary left;
  left.emplace("alpha", 1);
  left.emplace("beta", 2);

  StringIntDictionary same;
  same.emplace("alpha", 1);
  same.emplace("beta", 2);

  StringIntDictionary different_value;
  different_value.emplace("alpha", 1);
  different_value.emplace("beta", 20);

  StringIntDictionary different_order;
  different_order.emplace("beta", 2);
  different_order.emplace("alpha", 1);

  REQUIRE(left == same);
  REQUIRE_FALSE(left == different_value);
  REQUIRE_FALSE(left == different_order);

  StringIntDictionary right;
  right.emplace("gamma", 3);

  left.swap(right);
  REQUIRE(Keys(left) == std::vector<std::string>{"gamma"});
  REQUIRE(Keys(right) == std::vector<std::string>{"alpha", "beta"});

  swap(left, right);
  REQUIRE(Keys(left) == std::vector<std::string>{"alpha", "beta"});
  REQUIRE(Keys(right) == std::vector<std::string>{"gamma"});
}
