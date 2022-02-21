#include "cru/common/log/Logger.h"
#include "cru/common/log/StdioLogTarget.h"

#include <array>
#include <cstdlib>
#include <ctime>

namespace cru::log {
Logger *Logger::GetInstance() {
  static Logger logger;

  logger.AddLogTarget(std::make_unique<StdioLogTarget>());

  return &logger;
}

void Logger::AddLogTarget(std::unique_ptr<ILogTarget> target) {
  std::lock_guard<std::mutex> lock(target_list_mutex_);
  target_list_.push_back(std::move(target));
}

void Logger::RemoveLogTarget(ILogTarget *target) {
  std::lock_guard<std::mutex> lock(target_list_mutex_);
  target_list_.erase(
      std::remove_if(target_list_.begin(), target_list_.end(),
                     [target](const auto &t) { return t.get() == target; }),
      target_list_.end());
}

namespace {
String LogLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::Debug:
      return u"DEBUG";
    case LogLevel::Info:
      return u"INFO";
    case LogLevel::Warn:
      return u"WARN";
    case LogLevel::Error:
      return u"ERROR";
    default:
      std::terminate();
  }
}

String GetLogTime() {
  auto time = std::time(nullptr);
  auto calendar = std::localtime(&time);
  return Format(u"{}:{}:{}", calendar->tm_hour, calendar->tm_min,
                calendar->tm_sec);
}

String MakeLogFinalMessage(const LogInfo &log_info) {
  if (log_info.tag.empty()) {
    return Format(u"[{}] {}: {}", GetLogTime(),
                  LogLevelToString(log_info.level), log_info.message);
  } else {
    return Format(u"[{}] {} {}: {}", GetLogTime(),
                  LogLevelToString(log_info.level), log_info.tag,
                  log_info.message);
  }
}
}  // namespace

Logger::Logger()
    : log_thread_([this] {
        while (true) {
          auto log_info = log_queue_.Pull();
          std::lock_guard<std::mutex> lock_guard{target_list_mutex_};
          for (const auto &target : target_list_) {
            target->Write(log_info.level, MakeLogFinalMessage(log_info));
          }
        }
      }) {}

Logger::~Logger() {}

void Logger::Log(LogInfo log_info) {
#ifndef CRU_DEBUG
  if (log_info.level == LogLevel::Debug) {
    return;
  }
#endif
  log_queue_.Push(std::move(log_info));
}
}  // namespace cru::log
