#include "cru/platform/gui/win/UiApplication.h"

#include "cru/base/StringUtil.h"
#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/gui/SaveOpenDialogOptions.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/win/Base.h"
#include "cru/platform/gui/win/Clipboard.h"
#include "cru/platform/gui/win/Cursor.h"
#include "cru/platform/gui/win/Window.h"

#include <shobjidl.h>
#include <shobjidl_core.h>
#include <wrl/client.h>
#include <algorithm>
#include <chrono>
#include <format>

namespace cru::platform::gui::win {
namespace {
LRESULT __stdcall GeneralWndProc(HWND hWnd, UINT Msg, WPARAM wParam,
                                 LPARAM lParam) {
  auto* application = WinUiApplication::GetInstance();
  auto* window = application == nullptr ? nullptr : application->FromHWND(hWnd);

  LRESULT result;
  if (window != nullptr &&
      window->HandleNativeWindowMessage(hWnd, Msg, wParam, lParam, &result))
    return result;

  return DefWindowProc(hWnd, Msg, wParam, lParam);
}
}  // namespace

WinUiApplication::WinUiApplication() {
  instance_handle_ = ::GetModuleHandleW(nullptr);
  if (!instance_handle_)
    throw Win32Error("Failed to get module(instance) handle.");

  ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

  general_window_class_ = std::make_unique<WindowClass>(
      L"CruUIWindowClass", GeneralWndProc, instance_handle_);

  graphics_factory_ = std::make_unique<
      cru::platform::graphics::direct2d::DirectGraphicsFactory>();

  cursor_manager_ = std::make_unique<WinCursorManager>();
  clipboard_ = std::make_unique<WinClipboard>(this);
}

WinUiApplication::~WinUiApplication() { delete_later_pool_.Clean(); }

WinUiApplication* WinUiApplication::GetInstance() {
  return dynamic_cast<WinUiApplication*>(IUiApplication::GetInstance());
}

int WinUiApplication::Run() {
  MSG msg;
  bool exit = false;

  while (!exit) {
    if (auto result = timers_.Update(std::chrono::steady_clock::now())) {
      result->data();
      continue;
    }

    auto timeout = timers_.NextTimeout(std::chrono::steady_clock::now());

    ::MsgWaitForMultipleObjects(
        0, nullptr, FALSE, timeout ? timeout->count() : INFINITE, QS_ALLINPUT);

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);

      if (msg.message == WM_QUIT) {
        exit = true;
        break;
      }
    }

    delete_later_pool_.Clean();
  }

  for (const auto& handler : quit_handlers_) handler();

  return static_cast<int>(msg.wParam);
}

void WinUiApplication::RequestQuit(const int quit_code) {
  ::PostQuitMessage(quit_code);
}

void WinUiApplication::AddOnQuitHandler(std::function<void()> handler) {
  quit_handlers_.push_back(std::move(handler));
}

long long WinUiApplication::SetImmediate(std::function<void()> action) {
  return timers_.Add(std::move(action), std::chrono::milliseconds::zero(),
                     false);
}

long long WinUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                       std::function<void()> action) {
  return timers_.Add(std::move(action), milliseconds, false);
}

long long WinUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  return timers_.Add(std::move(action), milliseconds, true);
}

void WinUiApplication::CancelTimer(long long id) { timers_.Remove(id); }

void WinUiApplication::DeleteLater(Object* object) {
  delete_later_pool_.Add(object);
}

std::vector<INativeWindow*> WinUiApplication::GetAllWindow() {
  std::vector<INativeWindow*> result;
  for (const auto w : windows_) {
    result.push_back(static_cast<INativeWindow*>(w));
  }
  return result;
}

INativeWindow* WinUiApplication::CreateWindow() {
  return new WinNativeWindow(this);
}

cru::platform::graphics::IGraphicsFactory*
WinUiApplication::GetGraphicsFactory() {
  return graphics_factory_.get();
}

ICursorManager* WinUiApplication::GetCursorManager() {
  return cursor_manager_.get();
}

IClipboard* WinUiApplication::GetClipboard() { return clipboard_.get(); }

namespace {
void DialogSetupCommon(IFileDialog* dialog, const SaveDialogOptions& options) {
  if (!options.title.empty()) {
    auto title = string::ToUtf16WString(options.title);
    dialog->SetTitle(title.c_str());
  }

  if (!options.prompt.empty()) {
    auto prompt = string::ToUtf16WString(options.prompt);
    dialog->SetOkButtonLabel(prompt.c_str());
  }

  std::vector<COMDLG_FILTERSPEC> filters;
  std::vector<std::wstring> filter_display;
  std::vector<std::wstring> filter_pattern;

  // 添加用户定义的类型
  for (const auto& ext : options.allowed_file_types) {
    if (ext.empty()) continue;
    COMDLG_FILTERSPEC spec;
    auto w_ext = cru::string::ToUtf16WString(ext);
    filter_display.push_back(std::format(L"{} Files (*.{})", w_ext, w_ext));
    filter_pattern.push_back(L"*." + w_ext);
    spec.pszName = filter_display.back().c_str();
    spec.pszSpec = filter_pattern.back().c_str();
    filters.push_back(spec);
  }

  // 如果允许所有文件类型，追加 "*.*"
  if (options.allow_all_file_types) {
    COMDLG_FILTERSPEC allSpec;
    allSpec.pszName = L"All Files (*.*)";
    allSpec.pszSpec = L"*.*";
    filters.push_back(allSpec);
  }

  if (!filters.empty()) {
    dialog->SetFileTypes((UINT)filters.size(), filters.data());
  }
}
}  // namespace

std::optional<std::string> WinUiApplication::ShowSaveDialog(
    SaveDialogOptions options) {
  Microsoft::WRL::ComPtr<IFileSaveDialog> dialog = nullptr;
  CheckHResult(CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
                                IID_IFileSaveDialog, (void**)&dialog));

  DialogSetupCommon(dialog.Get(), options);

  auto hr = dialog->Show(NULL);
  if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
    return std::nullopt;
  } else {
    CheckHResult(hr);
  }

  std::string result;

  Microsoft::WRL::ComPtr<IShellItem> r = nullptr;
  CheckHResult(dialog->GetResult(&r));
  PWSTR path = nullptr;
  r->GetDisplayName(SIGDN_FILESYSPATH, &path);
  if (path) {
    result = cru::string::ToUtf8String(std::wstring_view(path));
    CoTaskMemFree(path);
  }

  return result;
}

std::optional<std::vector<std::string>> WinUiApplication::ShowOpenDialog(
    OpenDialogOptions options) {
  Microsoft::WRL::ComPtr<IFileOpenDialog> dialog = nullptr;
  CheckHResult(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                IID_IFileOpenDialog, (void**)&dialog));

  DialogSetupCommon(dialog.Get(), options);

  FILEOPENDIALOGOPTIONS fos = 0;
  if (options.allow_multiple_selection) {
    fos |= FOS_ALLOWMULTISELECT;
  }
  if (options.can_choose_directories) {
    fos |= FOS_PICKFOLDERS;
  }
  dialog->SetOptions(fos);

  auto hr = dialog->Show(NULL);
  if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
    return std::nullopt;
  } else {
    CheckHResult(hr);
  }

  std::vector<std::string> results;

  Microsoft::WRL::ComPtr<IShellItemArray> r = nullptr;
  CheckHResult(dialog->GetResults(&r));
  DWORD count = 0;
  r->GetCount(&count);
  for (DWORD i = 0; i < count; i++) {
    Microsoft::WRL::ComPtr<IShellItem> item = nullptr;
    r->GetItemAt(i, &item);
    if (item) {
      PWSTR path = nullptr;
      item->GetDisplayName(SIGDN_FILESYSPATH, &path);
      if (path) {
        results.push_back(cru::string::ToUtf8String(std::wstring_view(path)));
        CoTaskMemFree(path);
      }
    }
  }

  return results;
}

std::vector<WinNativeWindow*> WinUiApplication::GetAllWinWindow() {
  return windows_;
}

WinNativeWindow* WinUiApplication::FromHWND(HWND hwnd) {
  for (auto window : windows_) {
    if (window->GetWindowHandle() == hwnd) {
      return window;
    }
  }
  return nullptr;
}

WindowClass* WinUiApplication::GetGeneralWindowClass() {
  return general_window_class_.get();
}

void WinUiApplication::RegisterWindow(WinNativeWindow* window) {
  windows_.push_back(window);
}

void WinUiApplication::UnregisterWindow(WinNativeWindow* window) {
  windows_.erase(std::ranges::find(windows_, window));
}
}  // namespace cru::platform::gui::win
