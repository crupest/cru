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

  template <typename... TArgs>
  void Debug(const std::string_view& format, TArgs&&... args) {
#ifdef CRU_DEBUG
    Log(LogLevel::Debug, util::Format(format, std::forward<TArgs>(args)...));
#endif
  }

  template <typename... TArgs>
  void Info(const std::string_view& format, TArgs&&... args) {
    Log(LogLevel::Info, util::Format(format, std::forward<TArgs>(args)...));
  }

  template <typename... TArgs>
  void Warn(const std::string_view& format, TArgs&&... args) {
    Log(LogLevel::Warn, util::Format(format, std::forward<TArgs>(args)...));
  }

  template <typename... TArgs>
  void Error(const std::string_view& format, TArgs&&... args) {
    Log(LogLevel::Error, util::Format(format, std::forward<TArgs>(args)...));
  }

 public:
  std::list<std::unique_ptr<ILogSource>> sources_;
};

template <typename... TArgs>
void Debug(const std::string_view& format, TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(
      LogLevel::Debug, util::Format(format, std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void Info(const std::string_view& format, TArgs&&... args) {
  Logger::GetInstance()->Log(
      LogLevel::Info, util::Format(format, std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Warn(const std::string_view& format, TArgs&&... args) {
  Logger::GetInstance()->Log(
      LogLevel::Warn, util::Format(format, std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Error(const std::string_view& format, TArgs&&... args) {
  Logger::GetInstance()->Log(
      LogLevel::Error, util::Format(format, std::forward<TArgs>(args)...));
}
}  // namespace cru::log
