#pragma once
#include "../win_pre_config.hpp"

#include "cru/common/base.hpp"

#include <string>

namespace cru::platform::native::win {
class WindowClass : public Object {
 public:
  WindowClass(const std::wstring& name, WNDPROC window_proc,
              HINSTANCE h_instance);
  WindowClass(const WindowClass& other) = delete;
  WindowClass(WindowClass&& other) = delete;
  WindowClass& operator=(const WindowClass& other) = delete;
  WindowClass& operator=(WindowClass&& other) = delete;
  ~WindowClass() override = default;

  const wchar_t* GetName() const { return name_.c_str(); }

  ATOM GetAtom() const { return atom_; }

 private:
  std::wstring name_;
  ATOM atom_;
};
}  // namespace cru::win::native
