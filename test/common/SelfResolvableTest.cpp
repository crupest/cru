#include "cru/common/Base.h"
#include "cru/common/SelfResolvable.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>

namespace {
class SelfResolvableTestClassBase {
 public:
  SelfResolvableTestClassBase() = default;
  CRU_DELETE_COPY(SelfResolvableTestClassBase)
  CRU_DEFAULT_MOVE(SelfResolvableTestClassBase)
  virtual ~SelfResolvableTestClassBase() = default;
};

class SelfResolvableTestClass
    : public SelfResolvableTestClassBase,
      public cru::SelfResolvable<SelfResolvableTestClass> {
 public:
  SelfResolvableTestClass() : ptr_(new int(123)) {}
  CRU_DELETE_COPY(SelfResolvableTestClass)
  CRU_DEFAULT_MOVE(SelfResolvableTestClass)
  ~SelfResolvableTestClass() override = default;

 private:
  std::shared_ptr<int> ptr_;
};
}  // namespace

TEST_CASE("SelfResolvable resolver should work.", "[self-resolvable]") {
  SelfResolvableTestClass test_object;

  auto resolver = test_object.CreateResolver();
  REQUIRE(resolver.Resolve() == &test_object);

  auto resolver_copy = resolver;
  REQUIRE(resolver.Resolve() == &test_object);
  REQUIRE(resolver.Resolve() == &test_object);

  auto resolver_move = std::move(resolver_copy);
  REQUIRE(resolver.Resolve() == &test_object);
  REQUIRE(resolver_copy.IsValid() == false);
  REQUIRE(resolver_move.Resolve() == &test_object);
}

TEST_CASE("SelfResolvable object destructed should work.",
          "[self-resolvable]") {
  SelfResolvableTestClass* test_object = new SelfResolvableTestClass();

  auto resolver = test_object->CreateResolver();
  auto resolver_copy = resolver;

  delete test_object;

  REQUIRE(resolver.Resolve() == nullptr);
  REQUIRE(resolver_copy.Resolve() == nullptr);

  auto resolver_copy2 = resolver_copy;
  REQUIRE(resolver_copy2.Resolve() == nullptr);

  auto resolver_move = std::move(resolver_copy);
  REQUIRE(resolver_copy.IsValid() == false);
  REQUIRE(resolver_move.Resolve() == nullptr);
}

TEST_CASE("SelfResolvable object moved should work.", "[self-resolvable]") {
  SelfResolvableTestClass test_object;

  auto resolver = test_object.CreateResolver();
  auto resolver_copy = resolver;

  SelfResolvableTestClass moved_object = std::move(test_object);

  REQUIRE(resolver.Resolve() == &moved_object);
  REQUIRE(resolver_copy.Resolve() == &moved_object);

  auto resolver_copy2 = resolver_copy;
  REQUIRE(resolver_copy2.Resolve() == &moved_object);

  auto resolver_move = std::move(resolver_copy);
  REQUIRE(resolver_copy.IsValid() == false);
  REQUIRE(resolver_move.Resolve() == &moved_object);
}

TEST_CASE("SelfResolvable should work for casted type.", "[self-resolvable]") {
  auto test_object = new SelfResolvableTestClass();

  cru::ObjectResolver<SelfResolvableTestClassBase> base_resolver =
      test_object->CreateResolver();

  REQUIRE(base_resolver.Resolve() == test_object);

  auto base_resolver2 = base_resolver;
  REQUIRE(base_resolver2.Resolve() == test_object);

  auto base_resolver3 = std::move(base_resolver2);
  REQUIRE(base_resolver3.Resolve() == test_object);

  auto moved_object = new SelfResolvableTestClass(std::move(*test_object));
  delete test_object;

  REQUIRE(base_resolver.Resolve() == moved_object);
  REQUIRE(base_resolver3.Resolve() == moved_object);

  delete moved_object;

  REQUIRE(base_resolver.Resolve() == nullptr);
  REQUIRE(base_resolver3.Resolve() == nullptr);
}
