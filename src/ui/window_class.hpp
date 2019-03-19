#pragma once
#include "pre.hpp"

#include "system_headers.hpp"

#include "base.hpp"

namespace cru::ui {
class WindowClass : public Object {
 public:
  WindowClass(const String& name, WNDPROC window_proc, HINSTANCE h_instance);
  WindowClass(const WindowClass& other) = delete;
  WindowClass(WindowClass&& other) = delete;
  WindowClass& operator=(const WindowClass& other) = delete;
  WindowClass& operator=(WindowClass&& other) = delete;
  ~WindowClass() override = default;

  const wchar_t* GetName() const { return name_.c_str(); }

  ATOM GetAtom() const { return atom_; }

 private:
  String name_;
  ATOM atom_;
};
}  // namespace cru::ui
