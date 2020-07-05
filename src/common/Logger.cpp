#include "cru/common/Logger.hpp"

#include <array>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string_view>

namespace cru::log {
namespace {
Logger *CreateLogger() {
  const auto logger = new Logger();
  return logger;
}
}  // namespace

Logger *Logger::GetInstance() {
  static std::unique_ptr<Logger> logger{CreateLogger()};
  return logger.get();
}

void Logger::AddSource(std::unique_ptr<ILogSource> source) {
  sources_.push_back(std::move(source));
}

void Logger::RemoveSource(ILogSource *source) {
  sources_.remove_if([source](const std::unique_ptr<ILogSource> &s) {
    return s.get() == source;
  });
}

namespace {
std::u16string_view LogLevelToString(LogLevel level) {
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

std::u16string GetLogTime() {
  auto time = std::time(nullptr);
  auto calendar = std::localtime(&time);
  return fmt::format(u"{}:{}:{}", calendar->tm_hour, calendar->tm_min,
                     calendar->tm_sec);
}
}  // namespace

void Logger::Log(LogLevel level, std::u16string_view s) {
#ifndef CRU_DEBUG
  if (level == LogLevel::Debug) {
    return;
  }
#endif
  for (const auto &source : sources_) {
    source->Write(level, fmt::format(u"[{}] {}: {}\n", GetLogTime(),
                                     LogLevelToString(level), s));
  }
}

void Logger::Log(LogLevel level, std::u16string_view tag,
                 std::u16string_view s) {
#ifndef CRU_DEBUG
  if (level == LogLevel::Debug) {
    return;
  }
#endif
  for (const auto &source : sources_) {
    auto now = std::time(nullptr);
    source->Write(level, fmt::format(u"[{}] {} {}: {}\n", GetLogTime(),
                                     LogLevelToString(level), tag, s));
  }
}
}  // namespace cru::log
