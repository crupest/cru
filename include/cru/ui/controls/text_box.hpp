#pragma once
#include "../no_child_control.hpp"

namespace cru::ui::controls {
class TextBox : public NoChildControl {
 public:
  static constexpr std::string_view control_type = "TextBox";

 protected:
  TextBox();

 public:
  CRU_DELETE_COPY(TextBox)
  CRU_DELETE_MOVE(TextBox)

  ~TextBox() override;

  std::string_view GetControlType() const final { return control_type; }
};
}  // namespace cru::ui::controls
