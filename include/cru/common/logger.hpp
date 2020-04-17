#pragma once

#include "cru/common/base.hpp"
#include "cru/common/format.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <string_view>

namespace cru::log {

enum class LogLevel { Debug, Info, Warn, Error };

struct ILogSource : virtual Interface {
  // Write the string s. LogLevel is just a helper. It has no effect on the
  // content to write.
  virtual void Write(LogLevel level, const std::string_view& s) = 0;
};

class StdioLogSource : public virtual ILogSource {
 public:
  StdioLogSource() = default;

  CRU_DELETE_COPY(StdioLogSource)
  CRU_DELETE_MOVE(StdioLogSource)

  ~StdioLogSource() override = default;

  void Write(LogLevel level, const std::string_view& s) override {
    // TODO: Emmm... Since it is buggy to use narrow char in UTF-8 on Windows. I
    // think this implementation might be broken. (However, I didn't test it.)
    // Maybe, I should detect Windows and use wide char (And I didn't test this
    // either) or other more complicated implementation. Currently, I settled
    // with this.
    if (level == LogLevel::Error) {
      std::cerr << s;
    } else {
      std::cout << s;
    }
  }
};

class Logger : public Object {
 public:
  static Logger* GetInstance();

 public:
  Logger() = default;

  CRU_DELETE_COPY(Logger)
  CRU_DELETE_MOVE(Logger)

  ~Logger() override = default;

 public:
  void AddSource(std::unique_ptr<ILogSource> source);
  void RemoveSource(ILogSource* source);

 public:
  void Log(LogLevel level, const std::string_view& s);

 public:
  std::list<std::unique_ptr<ILogSource>> sources_;
};

template <typename... TArgs>
void Debug([[maybe_unused]] TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(LogLevel::Debug,
                             util::Format(std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void Info(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Info,
                             util::Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Warn(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Warn,
                             util::Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Error(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Error,
                             util::Format(std::forward<TArgs>(args)...));
}
}  // namespace cru::log
