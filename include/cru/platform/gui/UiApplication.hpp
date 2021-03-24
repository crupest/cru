#pragma once
#include "Base.hpp"

#include "cru/common/Bitmask.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace cru::platform::gui {
namespace details {
struct CreateWindowFlagTag;
}

using CreateWindowFlag = Bitmask<details::CreateWindowFlagTag>;

struct CreateWindowFlags {
  static constexpr CreateWindowFlag NoCaptionAndBorder{0b1};
};

// The entry point of a ui application.
struct IUiApplication : public virtual INativeResource {
 public:
  static IUiApplication* GetInstance() { return instance; }

 private:
  static IUiApplication* instance;

 protected:
  IUiApplication();

 public:
  ~IUiApplication() override;

  // Block current thread and run the message loop. Return the exit code when
  // message loop gets a quit message (possibly posted by method RequestQuit).
  virtual int Run() = 0;

  // Post a quit message with given quit code.
  virtual void RequestQuit(int quit_code) = 0;

  virtual void AddOnQuitHandler(std::function<void()> handler) = 0;

  // Timer id should always be positive (not 0) and never the same. So it's ok
  // to use negative value (or 0) to represent no timer.
  virtual long long SetImmediate(std::function<void()> action) = 0;
  virtual long long SetTimeout(std::chrono::milliseconds milliseconds,
                               std::function<void()> action) = 0;
  virtual long long SetInterval(std::chrono::milliseconds milliseconds,
                                std::function<void()> action) = 0;
  // Implementation should guarantee calls on timer id already canceled have no
  // effects and do not crash. Also canceling negative id or 0 should always
  // result in no-op.
  virtual void CancelTimer(long long id) = 0;

  virtual std::vector<INativeWindow*> GetAllWindow() = 0;

  INativeWindow* CreateWindow(INativeWindow* parent) {
    return this->CreateWindow(parent, CreateWindowFlag(0));
  };
  virtual INativeWindow* CreateWindow(INativeWindow* parent,
                                      CreateWindowFlag flags) = 0;

  virtual cru::platform::graphics::IGraphFactory* GetGraphFactory() = 0;

  virtual ICursorManager* GetCursorManager() = 0;
};

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

// Bootstrap from this.
std::unique_ptr<IUiApplication> CreateUiApplication();
}  // namespace cru::platform::gui
