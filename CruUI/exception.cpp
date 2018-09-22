#include "exception.h"

#include <fmt/format.h>

namespace cru
{
    inline std::string HResultMakeMessage(HRESULT h_result, std::optional<std::string> message)
    {
        if (message.has_value())
            return fmt::format("An HResultError is thrown. HRESULT: {:#08x}.\nAdditional message: {}\n", h_result, message.value());
        else
            return fmt::format("An HResultError is thrown. HRESULT: {:#08x}.\n", h_result);
    }

    HResultError::HResultError(HRESULT h_result, std::optional<std::string_view> additional_message)
        : runtime_error(HResultMakeMessage(h_result, std::nullopt)), h_result_(h_result)
    {

    }

    inline std::string Win32MakeMessage(DWORD error_code, std::optional<std::string> message)
    {
        if (message.has_value())
            return fmt::format("Last error code: {:#04x}.\nAdditional message: {}\n", error_code, message.value());
        else
            return fmt::format("Last error code: {:#04x}.\n", error_code);
    }

    Win32Error::Win32Error(DWORD error_code, std::optional<std::string_view> additional_message)
        : runtime_error(Win32MakeMessage(error_code, std::nullopt)), error_code_(error_code)
    {

    }
}
