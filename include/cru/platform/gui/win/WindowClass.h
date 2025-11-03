#pragma once
#include "Base.h"

#include <string>

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API WindowClass : public Object {
 public:
  WindowClass(std::wstring name, WNDPROC window_proc, HINSTANCE h_instance);
  ~WindowClass() override = default;

  const wchar_t* GetName() const { return name_.c_str(); }

  ATOM GetAtom() const { return atom_; }

 private:
  std::wstring name_;
  ATOM atom_;
};
}  // namespace cru::platform::gui::win
