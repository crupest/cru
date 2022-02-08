#pragma once
#include "Base.h"

#include "String.h"
#include "Format.h"

#include <list>
#include <memory>

namespace cru::log {

enum class LogLevel { Debug, Info, Warn, Error };

struct CRU_BASE_API ILogSource : virtual Interface {
  // Write the string s. LogLevel is just a helper. It has no effect on the
  // content to write.
  virtual void Write(LogLevel level, StringView s) = 0;
};

class CRU_BASE_API Logger : public Object {
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
  void Log(LogLevel level, StringView message);
  void Log(LogLevel level, StringView tag, StringView message);

 private:
  std::list<std::unique_ptr<ILogSource>> sources_;
};

// TODO: Remove argument evaluation in Debug.
template <typename... TArgs>
void Debug([[maybe_unused]] TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(LogLevel::Debug,
                             Format(std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void Info(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Info,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Warn(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Warn,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Error(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Error,
                             Format(std::forward<TArgs>(args)...));
}

// TODO: Remove argument evaluation in Debug.
template <typename... TArgs>
void TagDebug([[maybe_unused]] StringView tag,
              [[maybe_unused]] TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(LogLevel::Debug, tag,
                             Format(std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void TagInfo(StringView tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Info, tag,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void TagWarn(StringView tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Warn, tag,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void TagError(StringView tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Error, tag,
                             Format(std::forward<TArgs>(args)...));
}

class StdioLogSource : public Object, public virtual ILogSource {
 public:
  explicit StdioLogSource(bool use_lock = false);

  CRU_DELETE_COPY(StdioLogSource)
  CRU_DELETE_MOVE(StdioLogSource)

  ~StdioLogSource() override;

 public:
  void Write(LogLevel level, StringView s) override;

 private:
  bool use_lock_;
};
}  // namespace cru::log
