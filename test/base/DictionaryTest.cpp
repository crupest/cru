#include "cru/base/Dictionary.h"

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <ranges>
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

std::vector<std::string> ViewKeys(const StringIntDictionary& dictionary) {
  std::vector<std::string> keys;
  for (const std::string& key : dictionary.Keys()) {
    keys.push_back(key);
  }
  return keys;
}

std::vector<int> Values(const StringIntDictionary& dictionary) {
  std::vector<int> values;
  for (int value : dictionary.Values()) {
    values.push_back(value);
  }
  return values;
}

static_assert(std::same_as<StringIntDictionary::key_type, std::string>);
static_assert(std::same_as<StringIntDictionary::mapped_type, int>);
static_assert(std::same_as<StringIntDictionary::value_type,
                           std::pair<const std::string, int>>);
static_assert(std::is_const_v<StringIntDictionary::value_type::first_type>);
static_assert(std::is_constructible_v<
              StringIntDictionary,
              std::initializer_list<StringIntDictionary::value_type>>);
static_assert(std::bidirectional_iterator<StringIntDictionary::iterator>);
static_assert(std::bidirectional_iterator<StringIntDictionary::const_iterator>);
static_assert(
    std::bidirectional_iterator<StringIntDictionary::reverse_iterator>);
static_assert(
    std::bidirectional_iterator<StringIntDictionary::const_reverse_iterator>);
static_assert(std::random_access_iterator<StringIntDictionary::iterator>);
static_assert(std::random_access_iterator<StringIntDictionary::const_iterator>);
static_assert(
    std::random_access_iterator<StringIntDictionary::reverse_iterator>);
static_assert(
    std::random_access_iterator<StringIntDictionary::const_reverse_iterator>);
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
static_assert(std::ranges::range<
              decltype(std::declval<const StringIntDictionary&>().Keys())>);
static_assert(std::same_as<
              std::ranges::range_reference_t<
                  decltype(std::declval<const StringIntDictionary&>().Keys())>,
              const std::string&>);
static_assert(std::ranges::range<
              decltype(std::declval<StringIntDictionary&>().Values())>);
static_assert(
    std::same_as<std::ranges::range_reference_t<
                     decltype(std::declval<StringIntDictionary&>().Values())>,
                 int&>);
static_assert(
    std::same_as<
        std::ranges::range_reference_t<
            decltype(std::declval<const StringIntDictionary&>().Values())>,
        const int&>);
}  // namespace

TEST_CASE("Dictionary exposes vector-like container metadata", "[dictionary]") {
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

TEST_CASE("Dictionary initializer-list constructor copies ordered entries",
          "[dictionary]") {
  std::initializer_list<StringIntDictionary::value_type> init = {
      {"alpha", 1}, {"beta", 2}, {"gamma", 3}};

  StringIntDictionary dictionary(init);

  REQUIRE(dictionary.size() == 3);
  REQUIRE(dictionary.ssize() == 3);
  REQUIRE(Keys(dictionary) ==
          std::vector<std::string>{"alpha", "beta", "gamma"});
  REQUIRE(ViewKeys(dictionary) ==
          std::vector<std::string>{"alpha", "beta", "gamma"});
  REQUIRE(Values(dictionary) == std::vector<int>{1, 2, 3});

  REQUIRE(dictionary.begin()[1].first == "beta");
  REQUIRE(dictionary.begin()[1].second == 2);
  REQUIRE(dictionary.contains("gamma"));
  REQUIRE(dictionary.at("alpha") == 1);

  REQUIRE(&dictionary.begin()->first != &init.begin()->first);
  REQUIRE(&dictionary.begin()->second != &init.begin()->second);

  dictionary.at("alpha") = 10;
  REQUIRE(dictionary.at("alpha") == 10);
  REQUIRE(init.begin()->second == 1);

  const auto& const_dictionary = dictionary;
  REQUIRE(const_dictionary.begin()->first == "alpha");
  REQUIRE(Values(const_dictionary) == std::vector<int>{10, 2, 3});

  StringIntDictionary empty({});
  REQUIRE(empty.empty());
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

TEST_CASE("Dictionary copy constructor and assignment deep copy entries",
          "[dictionary]") {
  StringIntDictionary original;
  original.emplace("alpha", 1);
  original.emplace("beta", 2);

  StringIntDictionary constructed(original);
  REQUIRE(Keys(constructed) == std::vector<std::string>{"alpha", "beta"});
  REQUIRE(Values(constructed) == std::vector<int>{1, 2});

  auto original_alpha = original.find("alpha");
  auto constructed_alpha = constructed.find("alpha");
  REQUIRE(original_alpha != original.end());
  REQUIRE(constructed_alpha != constructed.end());
  REQUIRE(&constructed_alpha->second != &original_alpha->second);

  constructed.at("alpha") = 10;
  REQUIRE(original.at("alpha") == 1);
  REQUIRE(constructed.at("alpha") == 10);

  StringIntDictionary assigned;
  assigned.emplace("stale", 99);
  assigned = original;
  REQUIRE(Keys(assigned) == std::vector<std::string>{"alpha", "beta"});
  REQUIRE(Values(assigned) == std::vector<int>{1, 2});
  REQUIRE_FALSE(assigned.contains("stale"));

  auto original_beta = original.find("beta");
  auto assigned_beta = assigned.find("beta");
  REQUIRE(original_beta != original.end());
  REQUIRE(assigned_beta != assigned.end());
  REQUIRE(&assigned_beta->second != &original_beta->second);

  assigned.at("beta") = 20;
  REQUIRE(original.at("beta") == 2);
  REQUIRE(assigned.at("beta") == 20);

  assigned = assigned;
  REQUIRE(Keys(assigned) == std::vector<std::string>{"alpha", "beta"});
  REQUIRE(Values(assigned) == std::vector<int>{1, 20});
}

TEST_CASE("Dictionary move constructor and assignment transfer entries",
          "[dictionary]") {
  using MoveOnlyDictionary = cru::Dictionary<std::string, std::unique_ptr<int>>;

  MoveOnlyDictionary source;
  source.emplace("alpha", std::make_unique<int>(1));
  source.emplace("beta", std::make_unique<int>(2));
  int* alpha_value = source.find("alpha")->second.get();

  MoveOnlyDictionary constructed(std::move(source));
  REQUIRE(constructed.size() == 2);
  REQUIRE(constructed.find("alpha") != constructed.end());
  REQUIRE(constructed.find("beta") != constructed.end());
  REQUIRE(constructed.find("alpha")->second.get() == alpha_value);
  REQUIRE(*constructed.find("alpha")->second == 1);
  REQUIRE(*constructed.find("beta")->second == 2);

  MoveOnlyDictionary assigned;
  assigned.emplace("stale", std::make_unique<int>(99));
  assigned = std::move(constructed);
  REQUIRE(assigned.size() == 2);
  REQUIRE(assigned.find("stale") == assigned.end());
  REQUIRE(assigned.find("alpha") != assigned.end());
  REQUIRE(assigned.find("beta") != assigned.end());
  REQUIRE(assigned.find("alpha")->second.get() == alpha_value);
  REQUIRE(*assigned.find("alpha")->second == 1);
  REQUIRE(*assigned.find("beta")->second == 2);
}

TEST_CASE("Dictionary Keys and Values views expose ordered references",
          "[dictionary]") {
  StringIntDictionary dictionary;
  dictionary.emplace("alpha", 1);
  dictionary.emplace("beta", 2);
  dictionary.emplace("gamma", 3);

  REQUIRE(ViewKeys(dictionary) ==
          std::vector<std::string>{"alpha", "beta", "gamma"});
  REQUIRE(Values(dictionary) == std::vector<int>{1, 2, 3});

  auto keys = dictionary.Keys();
  auto values = dictionary.Values();
  REQUIRE(&*keys.begin() == &dictionary.begin()->first);
  REQUIRE(&*values.begin() == &dictionary.begin()->second);

  for (int& value : dictionary.Values()) {
    value *= 10;
  }
  REQUIRE(Values(dictionary) == std::vector<int>{10, 20, 30});
  REQUIRE(dictionary.at("beta") == 20);

  const StringIntDictionary& const_dictionary = dictionary;
  auto const_values = const_dictionary.Values();
  REQUIRE(ViewKeys(const_dictionary) ==
          std::vector<std::string>{"alpha", "beta", "gamma"});
  REQUIRE(Values(const_dictionary) == std::vector<int>{10, 20, 30});
  REQUIRE(&*const_values.begin() == &const_dictionary.begin()->second);
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
  const std::string* gamma_key = &gamma->first;
  int* gamma_value = &gamma->second;

  REQUIRE(dictionary.erase("missing") == 0);
  REQUIRE(dictionary.erase("beta") == 1);
  REQUIRE(*gamma_key == "gamma");
  REQUIRE(*gamma_value == 3);
  gamma = dictionary.find("gamma");
  REQUIRE(gamma != dictionary.end());
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
