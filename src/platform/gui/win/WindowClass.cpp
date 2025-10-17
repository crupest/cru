#include "cru/platform/gui/win/WindowClass.h"

#include "cru/platform/gui/win/Exception.h"

namespace cru::platform::gui::win {
WindowClass::WindowClass(std::wstring name, WNDPROC window_proc,
                         HINSTANCE h_instance)
    : name_(std::move(name)) {
  WNDCLASSEXW window_class;
  window_class.cbSize = sizeof(WNDCLASSEXW);

  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = window_proc;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = h_instance;
  window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
  window_class.lpszMenuName = NULL;
  window_class.lpszClassName = name_.c_str();
  window_class.hIconSm = NULL;

  atom_ = ::RegisterClassExW(&window_class);
  if (atom_ == 0)
    throw Win32Error(::GetLastError(), "Failed to create window class.");
}
}  // namespace cru::platform::gui::win
