#pragma once
#include "Logger.h"

namespace cru::log {
class StdioLogWriter : public Object, public virtual ILogWriter {
 public:
  explicit StdioLogWriter();
  ~StdioLogWriter() override;

 public:
  void Write(const LogInfo& log_info, std::string log_str) override;
};
}  // namespace cru::log
