#pragma once
#include "../../win_pre_config.hpp"

#include "cru/platform/exception.hpp"

#include <stdexcept>
#include <string_view>

namespace cru::platform::graph::win::direct {

class HResultError : public PlatformException {
 public:
  explicit HResultError(HRESULT h_result);
  explicit HResultError(HRESULT h_result,
                        const std::string_view& additional_message);
  HResultError(const HResultError& other) = default;
  HResultError(HResultError&& other) = default;
  HResultError& operator=(const HResultError& other) = default;
  HResultError& operator=(HResultError&& other) = default;
  ~HResultError() override = default;

  HRESULT GetHResult() const { return h_result_; }

 private:
  HRESULT h_result_;
};

inline void ThrowIfFailed(const HRESULT h_result) {
  if (FAILED(h_result)) throw HResultError(h_result);
}

inline void ThrowIfFailed(const HRESULT h_result,
                          const std::string_view& message) {
  if (FAILED(h_result)) throw HResultError(h_result, message);
}
}  // namespace cru::platform::graph::win::direct