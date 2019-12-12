#pragma once
#include "../win_pre_config.hpp"

#include "cru/common/base.hpp"

#include <string>

namespace cru::platform::native::win {
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
}  // namespace cru::platform::native::win
