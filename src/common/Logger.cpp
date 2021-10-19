#include "cru/common/Logger.hpp"

#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <mutex>
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
}  // namespace

void Logger::Log(LogLevel level, StringView message) {
#ifndef CRU_DEBUG
  if (level == LogLevel::Debug) {
    return;
  }
#endif
  for (const auto &source : sources_) {
    source->Write(level, Format(u"[{}] {}: {}\n", GetLogTime(),
                                LogLevelToString(level), message));
  }
}

void Logger::Log(LogLevel level, StringView tag, StringView message) {
#ifndef CRU_DEBUG
  if (level == LogLevel::Debug) {
    return;
  }
#endif
  for (const auto &source : sources_) {
    source->Write(level, Format(u"[{}] {} {}: {}\n", GetLogTime(),
                                LogLevelToString(level), tag, message));
  }
}

namespace {
std::mutex stdio_lock;

void WriteStdio(LogLevel level, StringView s) {
  std::string m = s.ToString().ToUtf8();

  if (level == LogLevel::Error) {
    std::cerr << m;
  } else {
    std::cout << m;
  }
}
}  // namespace

StdioLogSource::StdioLogSource(bool use_lock) : use_lock_(use_lock) {}

StdioLogSource::~StdioLogSource() {}

void StdioLogSource::Write(LogLevel level, StringView s) {
  if (use_lock_) {
    std::lock_guard<std::mutex> guard(stdio_lock);
    WriteStdio(level, s);
  } else {
    WriteStdio(level, s);
  }
}
}  // namespace cru::log
