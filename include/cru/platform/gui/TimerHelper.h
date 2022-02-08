#pragma once
#include "UiApplication.h"

namespace cru::platform::gui {

class TimerAutoCanceler {
 public:
  TimerAutoCanceler() : id_(0) {}
  explicit TimerAutoCanceler(long long id) : id_(id) {}

  CRU_DELETE_COPY(TimerAutoCanceler)

  TimerAutoCanceler(TimerAutoCanceler&& other) : id_(other.id_) {
    other.id_ = 0;
  }

  TimerAutoCanceler& operator=(TimerAutoCanceler&& other) {
    if (&other == this) {
      return *this;
    }
    Reset(other.id_);
    other.id_ = 0;
    return *this;
  }

  TimerAutoCanceler& operator=(long long other) {
    return this->operator=(TimerAutoCanceler(other));
  }

  ~TimerAutoCanceler() { Reset(); }

  long long Release() {
    auto temp = id_;
    id_ = 0;
    return temp;
  }

  void Reset(long long id = 0) {
    if (id_ > 0) IUiApplication::GetInstance()->CancelTimer(id_);
    id_ = id;
  }

  explicit operator bool() const { return id_; }

 private:
  long long id_;
};

class TimerListAutoCanceler {
 public:
  TimerListAutoCanceler() = default;
  CRU_DELETE_COPY(TimerListAutoCanceler)
  CRU_DEFAULT_MOVE(TimerListAutoCanceler)
  ~TimerListAutoCanceler() = default;

  TimerListAutoCanceler& operator+=(long long id) {
    list_.push_back(TimerAutoCanceler(id));
    return *this;
  }

  void Clear() { list_.clear(); }

  bool IsEmpty() const { return list_.empty(); }

 private:
  std::vector<TimerAutoCanceler> list_;
};

}  // namespace cru::platform::gui
