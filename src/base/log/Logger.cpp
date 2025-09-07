#include "cru/base/log/Logger.h"
#include "cru/base/log/StdioLogTarget.h"

#include <algorithm>
#include <ctime>
#include <format>

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/base/platform/win/DebugLogTarget.h"
#endif

namespace cru::log {
Logger *Logger::GetInstance() {
  static Logger logger;

  logger.AddLogTarget(std::make_unique<StdioLogTarget>());

#ifdef CRU_PLATFORM_WINDOWS
  logger.AddLogTarget(std::make_unique<platform::win::WinDebugLogTarget>());
#endif

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
const char *LogLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warn:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
    default:
      std::terminate();
  }
}

std::string GetLogTime() {
  auto time = std::time(nullptr);
  auto calendar = std::localtime(&time);
  return std::format("{}:{}:{}", calendar->tm_hour, calendar->tm_min,
                     calendar->tm_sec);
}

std::string MakeLogFinalMessage(const LogInfo &log_info) {
  return std::format("[{}] {} {}: {}\n", GetLogTime(),
                     LogLevelToString(log_info.level), log_info.tag,
                     log_info.message);
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

Logger::~Logger() { log_thread_.detach(); }

void Logger::Log(LogInfo log_info) {
#ifndef CRU_DEBUG
  if (log_info.level == LogLevel::Debug) {
    return;
  }
#endif
  log_queue_.Push(std::move(log_info));
}

LoggerCppStream::LoggerCppStream(Logger *logger, LogLevel level,
                                 std::string tag)
    : logger_(logger), level_(level), tag_(std::move(tag)) {}

LoggerCppStream LoggerCppStream::WithLevel(LogLevel level) const {
  return LoggerCppStream(this->logger_, level, this->tag_);
}

LoggerCppStream LoggerCppStream::WithTag(std::string tag) const {
  return LoggerCppStream(this->logger_, this->level_, std::move(tag));
}

void LoggerCppStream::Consume(std::string_view str) {
  this->logger_->Log(this->level_, this->tag_, std::string(str));
}
}  // namespace cru::log
