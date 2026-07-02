#include "cru/base/datamodel/DataValidator.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using cru::datamodel::AndDataValidator;
using cru::datamodel::DataValidateResult;
using cru::datamodel::FunctorDataValidator;
using cru::datamodel::NumberRangeDataValidator;

// ============================================================================
// DataValidateResult tests
// ============================================================================

TEST_CASE("DataValidateResult default construction", "[datamodel]") {
  DataValidateResult result;
  REQUIRE(result.IsValid());
  REQUIRE(result.GetErrors().empty());
}

TEST_CASE("DataValidateResult parameterized construction", "[datamodel]") {
  SECTION("valid with no errors") {
    DataValidateResult result(true, {});
    REQUIRE(result.IsValid());
    REQUIRE(result.GetErrors().empty());
  }

  SECTION("invalid with errors") {
    DataValidateResult result(false, {"error1", "error2"});
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors() == std::vector<std::string>{"error1", "error2"});
  }

  SECTION("valid with errors (warnings)") {
    DataValidateResult result(true, {"warning"});
    REQUIRE(result.IsValid());
    REQUIRE(result.GetErrors().size() == 1);
  }
}

TEST_CASE("DataValidateResult SetValid and AddError", "[datamodel]") {
  DataValidateResult result;

  result.SetValid(false);
  REQUIRE(!result.IsValid());

  result.AddError("first error");
  result.AddError("second error");
  REQUIRE(result.GetErrors() ==
          std::vector<std::string>{"first error", "second error"});

  result.SetValid(true);
  REQUIRE(result.IsValid());
  // errors should still be present after changing validity
  REQUIRE(result.GetErrors().size() == 2);
}

TEST_CASE("DataValidateResult::Success factory", "[datamodel]") {
  SECTION("no errors") {
    auto result = DataValidateResult::Success();
    REQUIRE(result.IsValid());
    REQUIRE(result.GetErrors().empty());
  }

  SECTION("with errors") {
    auto result = DataValidateResult::Success({"w1", "w2"});
    REQUIRE(result.IsValid());
    REQUIRE(result.GetErrors() == std::vector<std::string>{"w1", "w2"});
  }
}

TEST_CASE("DataValidateResult::SuccessWithError factory", "[datamodel]") {
  auto result = DataValidateResult::SuccessWithError("warning message");
  REQUIRE(result.IsValid());
  REQUIRE(result.GetErrors() == std::vector<std::string>{"warning message"});
}

TEST_CASE("DataValidateResult::Failure factory", "[datamodel]") {
  SECTION("single error string") {
    auto result = DataValidateResult::Failure("bad input");
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors() == std::vector<std::string>{"bad input"});
  }

  SECTION("multiple error strings") {
    auto result = DataValidateResult::Failure({"e1", "e2", "e3"});
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors() == std::vector<std::string>{"e1", "e2", "e3"});
  }
}

// ============================================================================
// FunctorDataValidator tests
// ============================================================================

TEST_CASE("FunctorDataValidator with DataValidateResult-returning function",
          "[datamodel]") {
  FunctorDataValidator<int> validator([](const int& value) {
    if (value > 0) {
      return DataValidateResult::Success();
    }
    return DataValidateResult::Failure("must be positive");
  });

  SECTION("valid value") {
    auto result = validator.Validate(5);
    REQUIRE(result.IsValid());
    REQUIRE(result.GetErrors().empty());
  }

  SECTION("invalid value") {
    auto result = validator.Validate(-3);
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors() == std::vector<std::string>{"must be positive"});
  }

  SECTION("boundary zero") {
    auto result = validator.Validate(0);
    REQUIRE(!result.IsValid());
  }
}

TEST_CASE("FunctorDataValidator with vector<string>-returning function",
          "[datamodel]") {
  FunctorDataValidator<std::string> validator(
      [](const std::string& value) -> std::vector<std::string> {
        std::vector<std::string> errors;
        if (value.empty()) {
          errors.push_back("must not be empty");
        }
        if (value.size() > 10) {
          errors.push_back("must not exceed 10 characters");
        }
        return errors;
      });

  SECTION("valid: no errors returned") {
    auto result = validator.Validate("hello");
    REQUIRE(result.IsValid());
    REQUIRE(result.GetErrors().empty());
  }

  SECTION("invalid: one error") {
    auto result = validator.Validate("");
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors() ==
            std::vector<std::string>{"must not be empty"});
  }

  SECTION("invalid: too long") {
    auto result = validator.Validate("this is way too long");
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors().size() == 1);
    REQUIRE(result.GetErrors()[0] == "must not exceed 10 characters");
  }

  SECTION("boundary: exactly 10 characters") {
    auto result = validator.Validate("0123456789");
    REQUIRE(result.IsValid());
  }
}

TEST_CASE("FunctorDataValidator with complex type", "[datamodel]") {
  FunctorDataValidator<double> validator([](const double& value) {
    if (value != value) {  // NaN check
      return DataValidateResult::Failure("value is NaN");
    }
    return DataValidateResult::Success();
  });

  REQUIRE(validator.Validate(3.14).IsValid());
  REQUIRE(
      !validator.Validate(std::numeric_limits<double>::quiet_NaN()).IsValid());
}

// ============================================================================
// AndDataValidator tests
// ============================================================================

namespace {
template <typename T>
FunctorDataValidator<T>* MakeValidator(
    std::function<DataValidateResult(const T&)> func) {
  return new FunctorDataValidator<T>(std::move(func));
}
}  // namespace

TEST_CASE("AndDataValidator combines positive results", "[datamodel]") {
  auto v1 = MakeValidator<int>([](const int& v) {
    return v > 0 ? DataValidateResult::Success()
                 : DataValidateResult::Failure("not positive");
  });
  auto v2 = MakeValidator<int>([](const int& v) {
    return v < 100 ? DataValidateResult::Success()
                   : DataValidateResult::Failure("not less than 100");
  });

  AndDataValidator<int> combined({v1, v2});

  auto result = combined.Validate(50);
  REQUIRE(result.IsValid());
  REQUIRE(result.GetErrors().empty());
}

TEST_CASE("AndDataValidator collects all errors", "[datamodel]") {
  auto v1 = MakeValidator<int>([](const int& v) {
    return DataValidateResult::Failure("error from v1");
  });
  auto v2 = MakeValidator<int>([](const int& v) {
    return DataValidateResult::Failure("error from v2");
  });
  auto v3 = MakeValidator<int>([](const int& v) {
    return DataValidateResult::SuccessWithError("warning from v3");
  });

  AndDataValidator<int> combined({v1, v2, v3});

  auto result = combined.Validate(0);
  REQUIRE(!result.IsValid());
  REQUIRE(result.GetErrors() == std::vector<std::string>{"error from v1",
                                                         "error from v2",
                                                         "warning from v3"});
}

TEST_CASE("AndDataValidator partial failure", "[datamodel]") {
  auto v1 = MakeValidator<int>(
      [](const int& v) { return DataValidateResult::Success(); });
  auto v2 = MakeValidator<int>([](const int& v) {
    return DataValidateResult::Failure("only v2 fails");
  });
  auto v3 = MakeValidator<int>(
      [](const int& v) { return DataValidateResult::Success(); });

  AndDataValidator<int> combined({v1, v2, v3});

  auto result = combined.Validate(0);
  REQUIRE(!result.IsValid());
  REQUIRE(result.GetErrors() == std::vector<std::string>{"only v2 fails"});
}

TEST_CASE("AndDataValidator with empty validator list", "[datamodel]") {
  AndDataValidator<int> combined({});
  auto result = combined.Validate(42);
  REQUIRE(result.IsValid());
  REQUIRE(result.GetErrors().empty());
}

TEST_CASE("AndDataValidator no auto-delete", "[datamodel]") {
  auto* v1 = MakeValidator<int>(
      [](const int& v) { return DataValidateResult::Success(); });

  {
    AndDataValidator<int> combined({v1}, /*auto_delete=*/false);
    auto result = combined.Validate(0);
    REQUIRE(result.IsValid());
  }
  // v1 should still be alive; clean up manually
  delete v1;
}

// ============================================================================
// NumberRangeDataValidator tests
// ============================================================================

TEST_CASE("NumberRangeDataValidator with both min and max inclusive",
          "[datamodel]") {
  NumberRangeDataValidator<int> validator(0, true, 100, true);

  SECTION("within range") {
    REQUIRE(validator.Validate(0).IsValid());
    REQUIRE(validator.Validate(50).IsValid());
    REQUIRE(validator.Validate(100).IsValid());
  }

  SECTION("below range") {
    auto result = validator.Validate(-1);
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors().size() == 1);
  }

  SECTION("above range") {
    auto result = validator.Validate(101);
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors().size() == 1);
  }
}

TEST_CASE("NumberRangeDataValidator with both min and max exclusive",
          "[datamodel]") {
  NumberRangeDataValidator<int> validator(0, false, 100, false);

  SECTION("within range") {
    REQUIRE(validator.Validate(1).IsValid());
    REQUIRE(validator.Validate(50).IsValid());
    REQUIRE(validator.Validate(99).IsValid());
  }

  SECTION("at boundaries") {
    REQUIRE(!validator.Validate(0).IsValid());
    REQUIRE(!validator.Validate(100).IsValid());
  }
}

TEST_CASE("NumberRangeDataValidator min-only", "[datamodel]") {
  SECTION("inclusive") {
    NumberRangeDataValidator<int> validator(10, true, std::nullopt, false);
    REQUIRE(validator.Validate(10).IsValid());
    REQUIRE(validator.Validate(1000).IsValid());
    REQUIRE(!validator.Validate(9).IsValid());
  }

  SECTION("exclusive") {
    NumberRangeDataValidator<int> validator(10, false, std::nullopt, false);
    REQUIRE(!validator.Validate(10).IsValid());
    REQUIRE(validator.Validate(11).IsValid());
  }
}

TEST_CASE("NumberRangeDataValidator max-only", "[datamodel]") {
  SECTION("inclusive") {
    NumberRangeDataValidator<int> validator(std::nullopt, false, 50, true);
    REQUIRE(validator.Validate(50).IsValid());
    REQUIRE(validator.Validate(-100).IsValid());
    REQUIRE(!validator.Validate(51).IsValid());
  }

  SECTION("exclusive") {
    NumberRangeDataValidator<int> validator(std::nullopt, false, 50, false);
    REQUIRE(!validator.Validate(50).IsValid());
    REQUIRE(validator.Validate(49).IsValid());
  }
}

TEST_CASE("NumberRangeDataValidator with floating point", "[datamodel]") {
  NumberRangeDataValidator<double> validator(0.0, true, 1.0, true);

  REQUIRE(validator.Validate(0.0).IsValid());
  REQUIRE(validator.Validate(0.5).IsValid());
  REQUIRE(validator.Validate(1.0).IsValid());
  REQUIRE(!validator.Validate(-0.001).IsValid());
  REQUIRE(!validator.Validate(1.001).IsValid());
}

TEST_CASE("NumberRangeDataValidator error message content", "[datamodel]") {
  SECTION("both bounds") {
    NumberRangeDataValidator<int> validator(1, true, 10, false);
    auto result = validator.Validate(15);
    REQUIRE(!result.IsValid());
    REQUIRE(!result.GetErrors()[0].empty());
  }

  SECTION("min only") {
    NumberRangeDataValidator<int> validator(5, false, std::nullopt, false);
    auto result = validator.Validate(0);
    REQUIRE(!result.IsValid());
    REQUIRE(!result.GetErrors()[0].empty());
  }

  SECTION("max only") {
    NumberRangeDataValidator<int> validator(std::nullopt, false, 100, true);
    auto result = validator.Validate(200);
    REQUIRE(!result.IsValid());
    REQUIRE(!result.GetErrors()[0].empty());
  }
}

TEST_CASE("NumberRangeDataValidator with unsigned type", "[datamodel]") {
  NumberRangeDataValidator<unsigned> validator(0u, true, 255u, true);

  REQUIRE(validator.Validate(0u).IsValid());
  REQUIRE(validator.Validate(128u).IsValid());
  REQUIRE(validator.Validate(255u).IsValid());
  REQUIRE(!validator.Validate(256u).IsValid());
}

// ============================================================================
// Integration / composition tests
// ============================================================================

TEST_CASE("AndDataValidator composed with NumberRangeDataValidator",
          "[datamodel]") {
  auto* range = new NumberRangeDataValidator<int>(1, true, 10, true);
  auto* even = MakeValidator<int>([](const int& v) {
    return v % 2 == 0 ? DataValidateResult::Success()
                      : DataValidateResult::Failure("must be even");
  });

  AndDataValidator<int> combined({range, even});

  SECTION("satisfies both") {
    auto result = combined.Validate(4);
    REQUIRE(result.IsValid());
  }

  SECTION("fails range only") {
    auto result = combined.Validate(12);
    REQUIRE(!result.IsValid());
  }

  SECTION("fails even only") {
    auto result = combined.Validate(7);
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors() == std::vector<std::string>{"must be even"});
  }

  SECTION("fails both") {
    auto result = combined.Validate(-1);
    REQUIRE(!result.IsValid());
    REQUIRE(result.GetErrors().size() == 2);
  }
}
