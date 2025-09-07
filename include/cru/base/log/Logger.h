#pragma once
#include "../Base.h"

#include "../String.h"
#include "../concurrent/ConcurrentQueue.h"

#include <format>  // IWYU pragma: keep
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace cru::log {
enum class LogLevel { Debug, Info, Warn, Error };

struct CRU_BASE_API LogInfo {
  LogInfo(LogLevel level, std::string tag, std::string message)
      : level(level), tag(std::move(tag)), message(std::move(message)) {}

  LogLevel level;
  std::string tag;
  std::string message;
};

struct CRU_BASE_API ILogTarget : virtual Interface {
  // Write the string s. LogLevel is just a helper. It has no effect on the
  // content to write.
  virtual void Write(LogLevel level, std::string s) = 0;
};

class CRU_BASE_API Logger : public Object2 {
 public:
  static Logger* GetInstance();

 public:
  Logger();
  ~Logger() override;

 public:
  void AddLogTarget(std::unique_ptr<ILogTarget> source);
  void RemoveLogTarget(ILogTarget* source);

 public:
  void Log(LogLevel level, std::string tag, std::string message) {
    Log(LogInfo(level, std::move(tag), std::move(message)));
  }
  void Log(LogInfo log_info);

 private:
  concurrent::ConcurrentQueue<LogInfo> log_queue_;

  std::mutex target_list_mutex_;
  std::vector<std::unique_ptr<ILogTarget>> target_list_;

  std::thread log_thread_;
};

class CRU_BASE_API LoggerCppStream : public Object2 {
 public:
  explicit LoggerCppStream(Logger* logger, LogLevel level, std::string tag);
  ~LoggerCppStream() override = default;

  LoggerCppStream WithLevel(LogLevel level) const;
  LoggerCppStream WithTag(std::string tag) const;

 private:
  void Consume(std::string_view str);

 public:
  LoggerCppStream& operator<<(std::string_view str) {
    this->Consume(str);
    return *this;
  }

  template <typename T>
  LoggerCppStream& operator<<(T&& arg) {
    this->Consume(ToString(std::forward<T>(arg)));
    return *this;
  }

 private:
  Logger* logger_;
  LogLevel level_;
  std::string tag_;
};

}  // namespace cru::log

#define CRU_LOG_TAG_DEBUG(...)                                             \
  cru::log::Logger::GetInstance()->Log(cru::log::LogLevel::Debug, kLogTag, \
                                       std::format(__VA_ARGS__))

#define CRU_LOG_TAG_INFO(...)                                             \
  cru::log::Logger::GetInstance()->Log(cru::log::LogLevel::Info, kLogTag, \
                                       std::format(__VA_ARGS__))

#define CRU_LOG_TAG_WARN(...)                                             \
  cru::log::Logger::GetInstance()->Log(cru::log::LogLevel::Warn, kLogTag, \
                                       std::format(__VA_ARGS__))

#define CRU_LOG_TAG_ERROR(...)                                             \
  cru::log::Logger::GetInstance()->Log(cru::log::LogLevel::Error, kLogTag, \
                                       std::format(__VA_ARGS__))
