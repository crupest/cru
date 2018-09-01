#pragma once

#include "system_headers.h"
#include <optional>

#include "base.h"


namespace cru {
	class HResultError : public std::runtime_error
	{
	public:
		explicit HResultError(HRESULT h_result);
		HResultError(HRESULT h_result, const std::string& message);
	    HResultError(const HResultError& other) = default;
	    HResultError(HResultError&& other) = default;
	    HResultError& operator=(const HResultError& other) = default;
	    HResultError& operator=(HResultError&& other) = default;
		~HResultError() override = default;

		HRESULT GetHResult() const
		{
            return h_result_;
		}

	private:
	    static std::string MakeMessage(HRESULT h_result, std::optional<std::string> message);

	private:
		HRESULT h_result_;
	};

	inline void ThrowIfFailed(const HRESULT h_result) {
		if (FAILED(h_result))
			throw HResultError(h_result);
	}

	inline void ThrowIfFailed(const HRESULT h_result, const std::string& message) {
		if (FAILED(h_result))
			throw HResultError(h_result, message);
	}
}
