#pragma once
#include "pre.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <typeindex>
#include "system_headers.hpp"

#include "base.hpp"

#ifdef CRU_DEBUG
#include <unordered_set>
#endif

namespace cru {
class Application;

namespace ui {
class WindowClass;
}

class GodWindow : public Object {
 public:
  explicit GodWindow(Application* application);
  GodWindow(const GodWindow& other) = delete;
  GodWindow(GodWindow&& other) = delete;
  GodWindow& operator=(const GodWindow& other) = delete;
  GodWindow& operator=(GodWindow&& other) = delete;
  ~GodWindow() override;

  HWND GetHandle() const { return hwnd_; }

  std::optional<LRESULT> HandleGodWindowMessage(HWND hwnd, int msg,
                                                WPARAM w_param, LPARAM l_param);

 private:
  std::unique_ptr<ui::WindowClass> god_window_class_;
  HWND hwnd_;
};

class Application : public Object {
 public:
  static Application* GetInstance();

 private:
  static Application* instance_;

 public:
  explicit Application(HINSTANCE h_instance);
  Application(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(const Application&) = delete;
  Application& operator=(Application&&) = delete;
  ~Application() override;

 public:
  int Run();
  void Quit(int quit_code);

  HINSTANCE GetInstanceHandle() const { return h_instance_; }

  GodWindow* GetGodWindow() const { return god_window_.get(); }

  // Resolve a singleton.
  // All singletons will be delete in reverse order of resolve.
  template <typename T,
            typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
  T* ResolveSingleton(const std::function<T*(Application*)>& creator) {
    const auto& index = std::type_index{typeid(T)};
    const auto find_result = singleton_map_.find(index);
    if (find_result != singleton_map_.cend())
      return static_cast<T*>(find_result->second);

#ifdef CRU_DEBUG
    const auto type_find_result = singleton_type_set_.find(index);
    if (type_find_result != singleton_type_set_.cend())
      throw std::logic_error(
          "The singleton of that type is being constructed. This may cause a "
          "dead recursion.");
    singleton_type_set_.insert(index);
#endif

    auto singleton = creator(this);
    singleton_map_.emplace(index, static_cast<Object*>(singleton));
    singleton_list_.push_back(singleton);
    return singleton;
  }

 private:
  HINSTANCE h_instance_;

  std::unique_ptr<GodWindow> god_window_;

  std::unordered_map<std::type_index, Object*> singleton_map_;
  std::list<Object*> singleton_list_;  // used for reverse destroy.
#ifdef CRU_DEBUG
  std::unordered_set<std::type_index>
      singleton_type_set_;  // used for detecting dead recursion.
#endif
};

void InvokeLater(const std::function<void()>& action);
}  // namespace cru
