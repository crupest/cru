#include "cru/platform/gui/UiApplication.h"
#include "cru/base/Base.h"

#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace cru::platform::gui {

void DeleteLaterPool::Add(Object* object) { pool_.push_back(object); }

void DeleteLaterPool::Clean() {
  // Destructors of objects might add more objects to delete later. So the safe
  // implementation is to copy current pool to avoid modification during
  // iteration.
  while (!pool_.empty()) {
    std::vector<Object*> copy = std::move(pool_);
    std::unordered_set<Object*> deleted;
    for (auto object : copy) {
      if (!deleted.contains(object)) {
        deleted.insert(object);
        delete object;
      }
    }
  }
}

namespace {
thread_local IUiApplication* instance = nullptr;

class InstanceRegistry {
 public:
  IUiApplication* Get(std::thread::id thread_id) {
    std::lock_guard lock(mutex_);
    auto iter = instances_.find(thread_id);
    return iter == instances_.cend() ? nullptr : iter->second;
  }

  void Set(std::thread::id thread_id, IUiApplication* application) {
    std::lock_guard lock(mutex_);
    if (application == nullptr) {
      instances_.erase(thread_id);
    } else {
      instances_[thread_id] = application;
    }
  }

 private:
  std::mutex mutex_;
  std::unordered_map<std::thread::id, IUiApplication*> instances_;
};

InstanceRegistry& GetInstanceRegistry() {
  // Function-local static fixes cross-TU initialization order: another static
  // constructor may call SetInstance before this TU's namespace-scope objects
  // would otherwise be initialized. Allocating the registry intentionally
  // avoids destruction: ScopedRegistration guards can run during thread-local
  // cleanup at shutdown and call SetInstance(nullptr), so the registry must
  // outlive normal static destruction order.
  static auto* registry = new InstanceRegistry;
  return *registry;
}
}  // namespace

IUiApplication::ScopedRegistration::ScopedRegistration(
    IUiApplication* application)
    : application_(application), previous_(GetInstance()), guard_([this] {
        if (GetInstance() == application_) {
          SetInstance(previous_);
        }
      }) {
  SetInstance(application);
}

IUiApplication::ScopedRegistration::ScopedRegistration(
    IUiApplication& application)
    : ScopedRegistration(&application) {}

IUiApplication* IUiApplication::GetInstance() { return instance; }

IUiApplication* IUiApplication::GetInstance(std::thread::id thread_id) {
  return GetInstanceRegistry().Get(thread_id);
}

void IUiApplication::SetInstance(IUiApplication* i) {
  instance = i;
  GetInstanceRegistry().Set(std::this_thread::get_id(), i);
}

IUiApplication::IUiApplication(bool register_instance) {
  if (register_instance) {
    instance_registration_.emplace(this);
  }
}

IMenu* IUiApplication::GetApplicationMenu() { return nullptr; }

std::optional<std::string> IUiApplication::ShowSaveDialog(
    SaveDialogOptions options) {
  NotImplemented();
}

std::optional<std::vector<std::string>> IUiApplication::ShowOpenDialog(
    OpenDialogOptions options) {
  NotImplemented();
}
}  // namespace cru::platform::gui
