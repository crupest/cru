#pragma once
#include "../controls/TextBox.h"
#include "Component.h"

namespace cru::ui::components {
struct CRU_UI_API InputValidateResult {
  bool valid;
  String message;
};

struct CRU_UI_API IInputValidator : public virtual Interface {
  virtual InputValidateResult Validate(StringView text) const = 0;
};

struct CRU_UI_API InputChangeEventArgs {
  String text;
  bool valid;
  String message;
};

class CRU_UI_API Input : public Component {
 public:
  Input();
  ~Input() override;

 public:
  controls::Control* GetRootControl() override;

  String GetText() const;
  void SetText(String text);

  IInputValidator* GetValidator() const;
  void SetValidator(IInputValidator* validator);

  InputValidateResult Validate();
  InputValidateResult GetLastValidateResult() const;

  IEvent<InputChangeEventArgs>* ChangeEvent() { return &change_event_; }

 private:
  controls::TextBox text_box_;
  IInputValidator* validator_;
  InputValidateResult last_validate_result_;

  Event<InputChangeEventArgs> change_event_;
};

class CRU_UI_API FloatInputValidator : public Object,
                                       public virtual IInputValidator {
 public:
  InputValidateResult Validate(StringView text) const override;

  std::optional<float> min;
  std::optional<float> max;
};

class CRU_UI_API FloatInput : public Input {
 public:
  FloatInput();
  ~FloatInput() override;

 public:
  float GetValue() const;
  void SetValue(float value);

  std::optional<float> GetMax() const;
  void SetMax(std::optional<float> max);

  std::optional<float> GetMin() const;
  void SetMin(std::optional<float> min);

 private:
  float value_;

  FloatInputValidator validator_;
};
}  // namespace cru::ui::components
