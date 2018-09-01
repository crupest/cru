#include "exception.h"

#include <sstream>
#include <iomanip>

namespace cru
{
	HResultError::HResultError(const HRESULT h_result)
		: runtime_error(MakeMessage(h_result, std::nullopt)), h_result_(h_result)
	{

	}

	HResultError::HResultError(const HRESULT h_result, const std::string& message)
		: runtime_error(MakeMessage(h_result, std::make_optional(message))), h_result_(h_result)
	{

	}

	std::string HResultError::MakeMessage(HRESULT h_result, std::optional<std::string> message)
	{
		std::stringstream ss;
		ss << "An HResultError is thrown. HRESULT: 0x" << std::setfill('0')
			<< std::setw(sizeof h_result * 2) << std::hex << h_result << ".";
		if (message.has_value())
			ss << "Additional message: " << message.value();
		return ss.str();
	}
}
