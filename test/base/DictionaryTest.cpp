#include "cru/base/Dictonary.h"

#include <catch2/catch_test_macros.hpp>

#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using cru::Dictionary;
using cru::DictionaryInsertResult;

namespace {

// A non-trivial value type: string + vector, default-constructible (for
// operator[]) and constructible from args (for emplace).
struct ComplexValue {
  std::string name;
  std::vector<int> data;

  ComplexValue() : name("<default>") {}
  ComplexValue(std::string name_, std::vector<int> data_)
      : name(std::move(name_)), data(std::move(data_)) {}
};

// A move-only value type: verifies no copies of the value happen.
struct MoveOnlyValue {
  std::unique_ptr<int> ptr;
  explicit MoveOnlyValue(int value) : ptr(std::make_unique<int>(value)) {}
  MoveOnlyValue(MoveOnlyValue&&) = default;
  MoveOnlyValue& operator=(MoveOnlyValue&&) = default;
  MoveOnlyValue(const MoveOnlyValue&) = delete;
  MoveOnlyValue& operator=(const MoveOnlyValue&) = delete;
};

// A key type that counts copies vs moves, to verify perfect forwarding of keys.
struct CountingKey {
  std::string value;
  inline static int copies = 0;
  inline static int moves = 0;

  CountingKey(std::string value_) : value(std::move(value_)) {}
  CountingKey(const CountingKey& other) : value(other.value) { ++copies; }
  CountingKey(CountingKey&& other) noexcept : value(std::move(other.value)) {
    ++moves;
  }
  CountingKey& operator=(const CountingKey&) = default;
  CountingKey& operator=(CountingKey&&) = default;
  bool operator==(const CountingKey& other) const {
    return value == other.value;
  }
};

}  // namespace

TEST_CASE("Dictionary default state", "[dictionary]") {
  Dictionary<std::string, int> dict;
  REQUIRE(dict.empty());
  REQUIRE(dict.size() == 0);
  REQUIRE(dict.begin() == dict.end());
  REQUIRE(dict.cbegin() == dict.cend());
  REQUIRE(dict.rbegin() == dict.rend());
  REQUIRE(dict.crbegin() == dict.crend());
  REQUIRE(dict.max_size() > 0);
}

TEST_CASE("Dictionary::operator[]", "[dictionary]") {
  Dictionary<std::string, int> dict;

  dict["a"] = 1;
  dict["b"] = 2;
  REQUIRE(dict.size() == 2);
  REQUIRE(dict["a"] == 1);
  REQUIRE(dict["b"] == 2);

  // Existing key: returns a reference to the same element, no new entry.
  dict["a"] = 10;
  REQUIRE(dict["a"] == 10);
  REQUIRE(dict.size() == 2);

  // Missing key: value-initializes a new entry (T{}).
  dict["c"];
  REQUIRE(dict["c"] == 0);
  REQUIRE(dict.size() == 3);

  // Returns a mutable reference.
  dict["d"] = 5;
  int& ref = dict["d"];
  ref = 50;
  REQUIRE(dict.at("d") == 50);

  dict["e"] = 7;
  REQUIRE(dict.at("e") == 7);
  REQUIRE(dict.size() == 5);
}

TEST_CASE("Dictionary::at", "[dictionary]") {
  Dictionary<std::string, int> dict;
  dict["a"] = 1;

  REQUIRE(dict.at("a") == 1);

  // Non-const overload returns a mutable reference.
  dict.at("a") = 2;
  REQUIRE(dict.at("a") == 2);

  const auto& cdict = dict;
  REQUIRE(cdict.at("a") == 2);

  REQUIRE_THROWS_AS(dict.at("missing"), std::out_of_range);
  REQUIRE_THROWS_AS(cdict.at("missing"), std::out_of_range);
}

TEST_CASE("Dictionary::find", "[dictionary]") {
  Dictionary<std::string, int> dict;
  dict["a"] = 1;
  dict["b"] = 2;

  // Heterogeneous lookup: const char* key against a std::string Key.
  auto it = dict.find("a");
  REQUIRE(it != dict.end());
  REQUIRE(it->first == "a");
  REQUIRE(it->second == 1);

  // Non-const iterator can mutate the mapped value.
  it->second = 10;
  REQUIRE(dict.at("a") == 10);

  REQUIRE(dict.find("missing") == dict.end());

  const auto& cdict = dict;
  auto cit = cdict.find("b");
  REQUIRE(cit != cdict.end());
  REQUIRE(cit->first == "b");
  REQUIRE(cit->second == 2);
  REQUIRE(cdict.find("missing") == cdict.end());
}

TEST_CASE("Dictionary::TryEmplace", "[dictionary]") {
  Dictionary<std::string, int> dict;

  REQUIRE(dict.TryEmplace("a", 1) == DictionaryInsertResult::Inserted);
  REQUIRE(dict.at("a") == 1);

  // Existing key is a no-op.
  REQUIRE(dict.TryEmplace("a", 100) == DictionaryInsertResult::None);
  REQUIRE(dict.at("a") == 1);
  REQUIRE(dict.size() == 1);

  // One explicit rvalue-key move (literals take the && overload too).
  std::string key = "b";
  REQUIRE(dict.TryEmplace(std::move(key), 2) ==
          DictionaryInsertResult::Inserted);
  REQUIRE(dict.at("b") == 2);
  REQUIRE(dict.size() == 2);

  // Value args are forwarded to the T constructor.
  Dictionary<std::string, std::string> sdict;
  REQUIRE(sdict.TryEmplace("k", 3, 'x') == DictionaryInsertResult::Inserted);
  REQUIRE(sdict.at("k") == "xxx");
}

TEST_CASE("Dictionary::ForceEmplace", "[dictionary]") {
  Dictionary<std::string, int> dict;

  REQUIRE(dict.ForceEmplace("a", 1) == DictionaryInsertResult::Inserted);
  REQUIRE(dict.at("a") == 1);

  // Existing key is overwritten.
  REQUIRE(dict.ForceEmplace("a", 2) == DictionaryInsertResult::Overwritten);
  REQUIRE(dict.at("a") == 2);
  REQUIRE(dict.size() == 1);

  REQUIRE(dict.ForceEmplace("b", 3) == DictionaryInsertResult::Inserted);
  REQUIRE(dict.at("b") == 3);
  REQUIRE(dict.size() == 2);
}

TEST_CASE("Dictionary::Remove", "[dictionary]") {
  Dictionary<std::string, int> dict;
  dict["a"] = 1;
  dict["b"] = 2;

  REQUIRE(dict.Remove("a"));
  REQUIRE(dict.size() == 1);
  REQUIRE(dict.find("a") == dict.end());
  REQUIRE(dict.at("b") == 2);

  REQUIRE_FALSE(dict.Remove("missing"));
  REQUIRE(dict.size() == 1);
}

TEST_CASE("Dictionary iterators", "[dictionary]") {
  Dictionary<std::string, int> dict;
  dict["a"] = 1;
  dict["b"] = 2;
  dict["c"] = 3;

  // Forward iteration (insertion order).
  int count = 0;
  int sum = 0;
  for (const auto& [key, value] : dict) {
    REQUIRE(!key.empty());
    sum += value;
    ++count;
  }
  REQUIRE(count == 3);
  REQUIRE(sum == 6);

  // Reverse iteration (insertion order reversed).
  auto rit = dict.rbegin();
  REQUIRE(rit != dict.rend());
  REQUIRE(rit->second == 3);
  ++rit;
  REQUIRE(rit->second == 2);
  ++rit;
  REQUIRE(rit->second == 1);
  ++rit;
  REQUIRE(rit == dict.rend());

  // Mutable iteration.
  for (auto it = dict.begin(); it != dict.end(); ++it) {
    it->second *= 10;
  }
  REQUIRE(dict.at("a") == 10);
  REQUIRE(dict.at("b") == 20);
  REQUIRE(dict.at("c") == 30);

  // Const iterators.
  const auto& cdict = dict;
  REQUIRE(std::distance(cdict.cbegin(), cdict.cend()) == 3);
  REQUIRE(std::distance(cdict.crbegin(), cdict.crend()) == 3);
}

TEST_CASE("Dictionary with complex value type", "[dictionary]") {
  Dictionary<std::string, ComplexValue> dict;

  dict["a"];  // value-initializes ComplexValue()
  REQUIRE(dict.at("a").name == "<default>");

  // Emplace forwards multiple args to the T constructor.
  REQUIRE(dict.TryEmplace("b", std::string("x"), std::vector{1, 2, 3}) ==
          DictionaryInsertResult::Inserted);
  REQUIRE(dict.at("b").name == "x");
  REQUIRE(dict.at("b").data == std::vector<int>({1, 2, 3}));

  // at returns a mutable reference into the stored value.
  dict.at("b").data.push_back(4);
  REQUIRE(dict.at("b").data == std::vector<int>({1, 2, 3, 4}));

  // ForceEmplace overwrites an existing entry.
  REQUIRE(dict.ForceEmplace("b", std::string("z"), std::vector<int>{9}) ==
          DictionaryInsertResult::Overwritten);
  REQUIRE(dict.size() == 2);
  REQUIRE(dict.at("b").name == "z");
  REQUIRE(dict.at("b").data == std::vector<int>({9}));
}

TEST_CASE("Dictionary forwards rvalue key and args to move-only value",
          "[dictionary]") {
  Dictionary<std::string, MoveOnlyValue> dict;

  REQUIRE(dict.TryEmplace("k", 42) == DictionaryInsertResult::Inserted);
  REQUIRE(*dict.at("k").ptr == 42);

  // at returns a mutable reference.
  dict.at("k").ptr = std::make_unique<int>(99);
  REQUIRE(*dict.at("k").ptr == 99);

  // Overwrite via ForceEmplace (move assignment of the value).
  REQUIRE(dict.ForceEmplace("k", 7) == DictionaryInsertResult::Overwritten);
  REQUIRE(*dict.at("k").ptr == 7);

  REQUIRE(dict.TryEmplace("j", 5) == DictionaryInsertResult::Inserted);
  REQUIRE(dict.size() == 2);
  REQUIRE(*dict.at("j").ptr == 5);
}

TEST_CASE("Dictionary perfect-forwards keys", "[dictionary]") {
  Dictionary<CountingKey, int> dict;

  // const& overload copies the key into storage.
  CountingKey::copies = 0;
  CountingKey::moves = 0;
  CountingKey lvalue_key("a");
  REQUIRE(dict.TryEmplace(lvalue_key, 1) == DictionaryInsertResult::Inserted);
  REQUIRE(CountingKey::copies == 1);
  REQUIRE(CountingKey::moves == 0);

  // && overload moves the key into storage.
  CountingKey::copies = 0;
  CountingKey::moves = 0;
  CountingKey rvalue_key("b");
  REQUIRE(dict.TryEmplace(std::move(rvalue_key), 2) ==
          DictionaryInsertResult::Inserted);
  REQUIRE(CountingKey::moves == 1);
  REQUIRE(CountingKey::copies == 0);

  REQUIRE(dict.size() == 2);
  REQUIRE(dict.at(CountingKey("a")) == 1);
  REQUIRE(dict.at(CountingKey("b")) == 2);
}
