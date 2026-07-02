#pragma once

#include "../Base.h"

#include <optional>
#include <string>
#include <vector>

namespace cru::datamodel {
class DataValidateResult {
 public:
  DataValidateResult() = default;
  DataValidateResult(bool valid, std::vector<std::string> errors)
      : valid_(valid), errors_(std::move(errors)) {}

  bool IsValid() const { return valid_; }
  const std::vector<std::string>& GetErrors() const { return errors_; }

  void SetValid(bool valid) { valid_ = valid; }
  void AddError(std::string error) { errors_.push_back(std::move(error)); }

  static DataValidateResult Success(std::vector<std::string> errors = {}) {
    return DataValidateResult(true, std::move(errors));
  }
  static DataValidateResult SuccessWithError(std::string error) {
    return DataValidateResult(true, {std::move(error)});
  }
  static DataValidateResult Failure(std::vector<std::string> errors) {
    return DataValidateResult(false, std::move(errors));
  }
  static DataValidateResult Failure(std::string error) {
    return DataValidateResult(false, {std::move(error)});
  }

 private:
  bool valid_ = true;
  std::vector<std::string> errors_;
};

struct IDataValidatorBase : virtual Interface {};

template <typename T>
struct IDataValidator : virtual IDataValidatorBase {
  virtual DataValidateResult Validate(const T& value) = 0;
};

template <typename T>
class DataValidatorBase : public Object, public virtual IDataValidator<T> {
 public:
  DataValidateResult Validate(const T& value) final {
    return DoValidate(value);
  }

 protected:
  virtual DataValidateResult DoValidate(const T& value) = 0;
};

template <typename T>
class FunctorDataValidator : public DataValidatorBase<T> {
 public:
  using ValidatorFunc = std::function<DataValidateResult(const T&)>;

  explicit FunctorDataValidator(ValidatorFunc func) : func_(std::move(func)) {}
  explicit FunctorDataValidator(
      std::function<std::vector<std::string>(const T&)> func)
      : func_([func = std::move(func)](const T& value) {
          auto errors = func(value);
          bool valid = errors.empty();
          return DataValidateResult(valid, std::move(errors));
        }) {}

 protected:
  DataValidateResult DoValidate(const T& value) override {
    return func_(value);
  }

 private:
  ValidatorFunc func_;
};

template <typename T>
class AndDataValidator : public DataValidatorBase<T> {
 public:
  explicit AndDataValidator(std::vector<IDataValidator<T>*> validators,
                            bool auto_delete = true)
      : validators_(std::move(validators)), auto_delete_(auto_delete) {}

  ~AndDataValidator() override {
    if (auto_delete_) {
      for (auto validator : validators_) {
        delete validator;
      }
    }
  }

 protected:
  DataValidateResult DoValidate(const T& value) override {
    DataValidateResult result(true, {});
    for (auto& validator : validators_) {
      auto sub_result = validator->Validate(value);
      if (!sub_result.IsValid()) {
        result.SetValid(false);
      }
      for (auto& error : sub_result.GetErrors()) {
        result.AddError(error);
      }
    }
    return result;
  }

 private:
  std::vector<IDataValidator<T>*> validators_;
  bool auto_delete_;
};

template <typename T>
  requires(std::is_arithmetic_v<T>)
class NumberRangeDataValidator : public DataValidatorBase<T> {
 public:
  NumberRangeDataValidator(std::optional<T> min, bool min_inclusive,
                           std::optional<T> max, bool max_inclusive)
      : min_(min),
        min_inclusive_(min_inclusive),
        max_(max),
        max_inclusive_(max_inclusive) {}

 protected:
  DataValidateResult DoValidate(const T& value) override {
    bool valid = true;
    if (min_) {
      if (min_inclusive_) {
        valid = value >= *min_;
      } else {
        valid = value > *min_;
      }
    }
    if (max_) {
      if (max_inclusive_) {
        valid = valid && value <= *max_;
      } else {
        valid = valid && value < *max_;
      }
    }
    return valid ? DataValidateResult::Success()
                 : DataValidateResult::Failure(MakeMessage());
  }

 private:
  std::string MakeMessage() const {
    auto inclusive_str = [](bool inclusive) {
      return inclusive ? "inclusive" : "exclusive";
    };

    if (min_ && max_) {
      return "Value must be between " + std::to_string(*min_) + " (" +
             inclusive_str(min_inclusive_) + ") and " + std::to_string(*max_) +
             " (" + inclusive_str(max_inclusive_) + ").";
    } else if (min_) {
      return "Value must be greater than " + std::to_string(*min_) + " (" +
             inclusive_str(min_inclusive_) + ").";
    } else if (max_) {
      return "Value must be less than " + std::to_string(*max_) + " (" +
             inclusive_str(max_inclusive_) + ").";
    } else {
      return "";
    }
  }

 private:
  std::optional<T> min_;
  bool min_inclusive_;
  std::optional<T> max_;
  bool max_inclusive_;
};
}  // namespace cru::datamodel
