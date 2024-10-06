#pragma once
#include "../Base.h"

#include "../Format.h"
#include "../String.h"
#include "../concurrent/ConcurrentQueue.h"

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace cru::log {
enum class LogLevel { Debug, Info, Warn, Error };

struct CRU_BASE_API LogInfo {
  LogInfo(LogLevel level, String tag, String message)
      : level(level), tag(std::move(tag)), message(std::move(message)) {}

  CRU_DEFAULT_COPY(LogInfo)
  CRU_DEFAULT_MOVE(LogInfo)

  ~LogInfo() = default;

  LogLevel level;
  String tag;
  String message;
};

struct CRU_BASE_API ILogTarget : virtual Interface {
  // Write the string s. LogLevel is just a helper. It has no effect on the
  // content to write.
  virtual void Write(LogLevel level, StringView s) = 0;
};

class CRU_BASE_API Logger : public Object {
 public:
  static Logger* GetInstance();

 public:
  Logger();

  CRU_DELETE_COPY(Logger)
  CRU_DELETE_MOVE(Logger)

  ~Logger() override;

 public:
  void AddLogTarget(std::unique_ptr<ILogTarget> source);
  void RemoveLogTarget(ILogTarget* source);

 public:
  void Log(LogLevel level, String tag, String message) {
    Log(LogInfo(level, std::move(tag), std::move(message)));
  }
  void Log(LogInfo log_info);

  template <typename... Args>
  void FormatLog(LogLevel level, String tag, StringView format,
                 Args&&... args) {
    Log(level, std::move(tag), Format(format, std::forward<Args>(args)...));
  }

 private:
  concurrent::ConcurrentQueue<LogInfo> log_queue_;

  std::mutex target_list_mutex_;
  std::vector<std::unique_ptr<ILogTarget>> target_list_;

  std::thread log_thread_;
};
}  // namespace cru::log

#define CRU_LOG_DEBUG(...)                                              \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Debug, \
                                             kLogTag, __VA_ARGS__)

#define CRU_LOG_INFO(...)                                              \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Info, \
                                             kLogTag, __VA_ARGS__)

#define CRU_LOG_WARN(...)                                              \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Warn, \
                                             kLogTag, __VA_ARGS__)

#define CRU_LOG_ERROR(...)                                              \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Error, \
                                             kLogTag, __VA_ARGS__)
