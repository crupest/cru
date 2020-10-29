#pragma once
#include "Base.hpp"

#include <string>

namespace cru::platform::gui::win {
class WindowClass : public Object {
 public:
  WindowClass(std::wstring name, WNDPROC window_proc, HINSTANCE h_instance);

  CRU_DELETE_COPY(WindowClass)
  CRU_DELETE_MOVE(WindowClass)

  ~WindowClass() override = default;

  const wchar_t* GetName() const { return name_.c_str(); }

  ATOM GetAtom() const { return atom_; }

 private:
  std::wstring name_;
  ATOM atom_;
};
}  // namespace cru::platform::gui::win
