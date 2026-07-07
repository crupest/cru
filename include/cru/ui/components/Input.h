#pragma once
#include "../ThemeManager.h"
#include "../controls/FlexLayout.h"
#include "../controls/TextBlock.h"
#include "../controls/TextBox.h"
#include "Component.h"

#include <cru/base/Event.h>
#include <cru/base/Guard.h>
#include <cru/base/StringUtil.h>
#include <cru/base/datamodel/DataType.h>
#include <cru/base/datamodel/DataValidator.h>
#include <cru/platform/gui/UiApplication.h>

#include <optional>
#include <string>
#include <vector>

namespace cru::ui::components {
template <typename T>
struct InputChangeEventArgs {
  std::string text;
  std::optional<T> value;
  bool valid;
  std::vector<std::string> errors;

  void AddErrors(const std::vector<std::string>& new_errors) {
    errors.insert(errors.cend(), new_errors.cbegin(), new_errors.cend());
  }
};

template <typename T>
class Input : public Component {
 public:
  static Input* Create(std::string label,
                       datamodel::IDataType<T>* data_type = nullptr,
                       datamodel::IDataValidator<T>* validator = nullptr) {
    auto input = new Input();
    input->SetLabelText(std::move(label));
    input->SetDataType(data_type);
    input->SetValidator(validator);
    return input;
  }

  Input()
      : in_internal_update_(false),
        dirty_(false),
        data_type_(nullptr),
        data_type_auto_delete_(true),
        validator_(nullptr),
        validator_auto_delete_(true) {
    root_layout_.SetFlexDirection(controls::FlexDirection::Vertical);
    root_layout_.AddChild(&label_text_block_);
    root_layout_.SetChildLayoutData(
        &label_text_block_,
        {.cross_alignment = controls::FlexCrossAlignment::Center});
    root_layout_.AddChild(&text_box_);

    error_text_block_.SetFont(ThemeManager::GetInstance()->GetResourceFont(
        "component.input.error.font"));
    error_text_block_.SetTextBrush(
        ThemeManager::GetInstance()->GetResourceBrush(
            "component.input.error.brush"));

    UpdateState();

    event_guard_ += text_box_.TextChangeEvent()->AddSpyOnlyHandler([this] {
      if (!in_internal_update_) {
        dirty_ = true;
        this->UpdateState();
      }
    });
  }

  ~Input() override {
    if (data_type_auto_delete_) {
      delete data_type_;
    }
    if (validator_auto_delete_) {
      delete validator_;
    }
  }

 public:
  controls::Control* GetRootControl() override { return &root_layout_; }

  std::string GetLabelText() { return label_text_block_.GetText(); }

  void SetLabelText(std::string label) {
    label_text_block_.SetText(std::move(label));
  }

  std::string GetText() { return text_box_.GetText(); }

  /**
   * @brief Set text of the input. Make the input not dirty any more.
   */
  void SetText(std::string text) {
    auto _ = StartInternalUpdate();
    dirty_ = false;
    text_box_.SetText(text);
    last_state_.text = text;
    last_state_.value = std::nullopt;
    last_state_.valid = true;
    last_state_.errors.clear();
    SyncErrorText();
  }

  std::optional<T> GetValue() { return last_state_.value; }

  void SetValue(std::optional<T> value) {
    auto _ = StartInternalUpdate();
    dirty_ = false;

    if (!value) {
      // We leave last_state_.text as is.
      last_state_.value = std::nullopt;
      last_state_.valid = true;
      last_state_.errors.clear();
      SyncErrorText();
      return;
    }

    if (!data_type_) {
      throw Exception("Data type is not set.");
    }

    if (!data_type_->SupportConvertToString()) {
      throw Exception("Data type does not support converting to string.");
    }

    last_state_.text = data_type_->ConvertToString(*value).GetValue();
    last_state_.value = value;
    last_state_.valid = true;
    last_state_.errors.clear();
    text_box_.SetText(last_state_.text);
    SyncErrorText();
  }

  datamodel::IDataType<T>* GetDataType() { return data_type_; }

  void SetDataType(datamodel::IDataType<T>* data_type) {
    if (!data_type->SupportConvertFromString()) {
      throw Exception("Data type does not support converting from string.");
    }

    data_type_ = data_type;
    UpdateState();
  }

  void SetDataType(datamodel::IDataType<T>* data_type, bool auto_delete) {
    data_type_ = data_type;
    data_type_auto_delete_ = auto_delete;
    UpdateState();
  }

  bool IsAutoDeleteDataType() { return data_type_auto_delete_; }

  void SetAutoDeleteDataType(bool auto_delete) {
    data_type_auto_delete_ = auto_delete;
  }

  datamodel::IDataValidator<T>* GetValidator() { return validator_; }

  void SetValidator(datamodel::IDataValidator<T>* validator) {
    validator_ = validator;
    UpdateState();
  }

  void SetValidator(datamodel::IDataValidator<T>* validator, bool auto_delete) {
    validator_ = validator;
    validator_auto_delete_ = auto_delete;
    UpdateState();
  }

  bool IsAutoDeleteValidator() { return validator_auto_delete_; }

  void SetAutoDeleteValidator(bool auto_delete) {
    validator_auto_delete_ = auto_delete;
  }

  IEvent<const InputChangeEventArgs<T>&>* ChangeEvent() {
    return &change_event_;
  }

 private:
  void UpdateState() {
    if (!dirty_) return;

    auto text = text_box_.GetText();
    last_state_.text = text;
    last_state_.value = std::nullopt;
    last_state_.valid = true;
    last_state_.errors.clear();

    if (data_type_) {
      auto convert_result = data_type_->ConvertFromString(text);
      last_state_.valid = convert_result.IsSuccess();
      last_state_.value = convert_result.GetOptionalValue();
      last_state_.AddErrors(convert_result.GetErrors());

      if (last_state_.valid && validator_) {
        auto validate_result = validator_->Validate(convert_result.GetValue());
        last_state_.valid = validate_result.IsValid();
        last_state_.AddErrors(validate_result.GetErrors());
      }
    }

    SyncErrorText();
    change_event_.Raise(last_state_);
  }

  void SyncErrorText() {
    if (last_state_.errors.empty()) {
      root_layout_.RemoveChild(&error_text_block_);
    } else {
      std::string error_text = cru::string::Join("\n", last_state_.errors);
      error_text_block_.SetText(error_text);
      if (!root_layout_.HasChild(&error_text_block_)) {
        root_layout_.AddChild(&error_text_block_);
      }
    }
  }

  Guard StartInternalUpdate() {
    in_internal_update_ = true;
    return Guard([this] { in_internal_update_ = false; });
  }

 private:
  controls::FlexLayout root_layout_;
  controls::TextBlock label_text_block_;
  controls::TextBox text_box_;
  controls::TextBlock error_text_block_;

  bool in_internal_update_;
  bool dirty_;
  datamodel::IDataType<T>* data_type_;
  bool data_type_auto_delete_;
  datamodel::IDataValidator<T>* validator_;
  bool validator_auto_delete_;

  InputChangeEventArgs<T> last_state_;

  EventHandlerRevokerListGuard event_guard_;

  Event<const InputChangeEventArgs<T>&> change_event_;
};
}  // namespace cru::ui::components
