#pragma once
#include "Logger.h"

namespace cru::log {
class StdioLogTarget : public Object, public virtual log::ILogTarget {
 public:
  explicit StdioLogTarget();


  ~StdioLogTarget() override;

 public:
  void Write(log::LogLevel level, StringView s) override;
};
}  // namespace cru::log
