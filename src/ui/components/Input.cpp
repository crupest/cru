#include "cru/ui/components/Input.h"
#include <cmath>
#include <optional>
#include "cru/common/StringToNumberConverter.h"
#include "cru/ui/controls/Control.h"

namespace cru::ui::components {
Input::Input() : last_validate_result_{true, u"Good value"} {
  text_box_.TextChangeEvent()->AddSpyOnlyHandler([this] {
    auto text = text_box_.GetText();
    auto validate_result = Validate();
    this->change_event_.Raise({std::move(text), validate_result.valid,
                               std::move(validate_result.message)});
  });
}

Input::~Input() {}

controls::Control* Input::GetRootControl() { return &text_box_; }

String Input::GetText() const { return text_box_.GetText(); }

void Input::SetText(String text) { text_box_.SetText(std::move(text)); }

IInputValidator* Input::GetValidator() const { return validator_; }

void Input::SetValidator(IInputValidator* validator) {
  validator_ = validator;
  Validate();
}

InputValidateResult Input::Validate() {
  if (validator_)
    last_validate_result_ = validator_->Validate(text_box_.GetTextView());
  else
    last_validate_result_ = {true, u"Good value"};
  return last_validate_result_;
}

InputValidateResult Input::GetLastValidateResult() const {
  return last_validate_result_;
}

InputValidateResult FloatInputValidator::Validate(StringView text) const {
  auto result =
      text.ParseToFloat(nullptr, StringToNumberFlags::kAllowLeadingSpaces &
                                     StringToNumberFlags::kAllowTrailingSpaces);
  if (std::isnan(result)) {
    return InputValidateResult{false, u"Invalid number."};
  }

  if (min && result < *min) {
    return InputValidateResult{false, u"Value is less than minimum."};
  }

  if (max && result > *max) {
    return InputValidateResult{false, u"Value is greater than maximum."};
  }

  return InputValidateResult{true, u"Good number"};
}

FloatInput::FloatInput() {
  SetValidator(&validator_);

  ChangeEvent()->AddHandler([this](const InputChangeEventArgs& args) {
    if (args.valid) {
      value_ = args.text.ParseToFloat(
          nullptr, StringToNumberFlags::kAllowLeadingSpaces &
                       StringToNumberFlags::kAllowTrailingSpaces);
    }
  });
}

FloatInput::~FloatInput() {}

float FloatInput::GetValue() const { return value_; }

void FloatInput::SetValue(float value) { SetText(ToString(value)); }

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
