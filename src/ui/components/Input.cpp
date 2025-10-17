#include "cru/ui/components/Input.h"
#include "cru/base/StringToNumberConverter.h"
#include "cru/ui/controls/Control.h"

#include <cmath>
#include <optional>
#include <string>

namespace cru::ui::components {
Input::Input() : last_validate_result_{true, "Good value"} {
  text_box_.TextChangeEvent()->AddSpyOnlyHandler([this] {
    auto text = text_box_.GetText();
    auto validate_result = Validate();
    this->change_event_.Raise({std::move(text), validate_result.valid,
                               std::move(validate_result.message)});
  });
}

Input::~Input() {}

controls::Control* Input::GetRootControl() { return &text_box_; }

std::string Input::GetText() const { return text_box_.GetText(); }

void Input::SetText(std::string text) { text_box_.SetText(std::move(text)); }

IInputValidator* Input::GetValidator() const { return validator_; }

void Input::SetValidator(IInputValidator* validator) {
  validator_ = validator;
  Validate();
}

InputValidateResult Input::Validate() {
  if (validator_)
    last_validate_result_ = validator_->Validate(text_box_.GetTextView());
  else
    last_validate_result_ = {true, "Good value"};
  return last_validate_result_;
}

InputValidateResult Input::GetLastValidateResult() const {
  return last_validate_result_;
}

InputValidateResult FloatInputValidator::Validate(std::string_view text) const {
  auto result = String::FromUtf8(text).ParseToFloat(
      nullptr, StringToNumberFlags::kAllowLeadingSpaces &
                   StringToNumberFlags::kAllowTrailingSpaces);
  if (std::isnan(result)) {
    return InputValidateResult{false, "Invalid number."};
  }

  if (min && result < *min) {
    return InputValidateResult{false, "Value is less than minimum."};
  }

  if (max && result > *max) {
    return InputValidateResult{false, "Value is greater than maximum."};
  }

  return InputValidateResult{true, "Good number"};
}

FloatInput::FloatInput() {
  SetValidator(&validator_);

  ChangeEvent()->AddHandler([this](const InputChangeEventArgs& args) {
    if (args.valid) {
      value_ = String::FromUtf8(args.text).ParseToFloat(
          nullptr, StringToNumberFlags::kAllowLeadingSpaces &
                       StringToNumberFlags::kAllowTrailingSpaces);
    }
  });
}

FloatInput::~FloatInput() {}

float FloatInput::GetValue() const { return value_; }

void FloatInput::SetValue(float value) { SetText(std::to_string(value)); }

std::optional<float> FloatInput::GetMin() const { return validator_.min; }

void FloatInput::SetMin(std::optional<float> min) {
  validator_.min = std::move(min);
  Validate();
}

std::optional<float> FloatInput::GetMax() const { return validator_.max; }

void FloatInput::SetMax(std::optional<float> max) {
  validator_.max = std::move(max);
  Validate();
}
}  // namespace cru::ui::components
