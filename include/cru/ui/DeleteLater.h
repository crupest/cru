#pragma once

#include <memory>
#include <utility>

namespace cru::ui {
class DeleteLaterImpl {
 public:
  virtual ~DeleteLaterImpl();
  void DeleteLater();
};

template <typename T>
struct DeleteLaterDeleter {
  void operator()(T* p) const { p->DeleteLater(); }
};

template <typename T>
using DeleteLaterPtr = std::unique_ptr<T, DeleteLaterDeleter<T>>;

template <typename T, typename... Args>
DeleteLaterPtr<T> CreateDeleteLaterPtr(Args&&... args) {
  return DeleteLaterPtr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace cru::ui
